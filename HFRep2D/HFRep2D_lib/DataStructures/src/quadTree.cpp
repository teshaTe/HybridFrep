#include "quadTree.h"
#include "timer.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>

namespace hfrep2D {

void quadTree::generateQuadTree(const std::vector<float> *inField, const glm::vec2 surfLim )
{
    /* 1. subdivide root node
     * 2. get value from field
     * 3. get quadrant for it
     * 4. compute box for it
     * 5. insert it in node
     */

    timer time;
    time.Start();

    surf_lim = surfLim;
    for(int y = 0; y < res_y; y++)
    {
        for(int x = 0; x < res_x; x++)
        {
            insertNode( glm::f32vec2(x,y), inField->at(x+y*res_x) );
        }
    }

    time.End("quadtree building time: ");
}

boundingBox quadTree::computeBox(const boundingBox &box, int i) const
{
    //the initial coord of the bounding box that will be subdivied
    auto origin = box.getTopLeft();
    //new size of the bounding box for the child
    auto childSize = box.getSize() / static_cast<glm::f32>(2);
    //based on index i choosing which node will be subdivided next
    switch(i)
    {
        case 0: // North west node
            return boundingBox( origin, childSize );
        case 1: // North east node
            return boundingBox( glm::f32vec2( origin.x+childSize.x, origin.y ), childSize );
        case 2: // South west node
            return boundingBox( glm::f32vec2( origin.x, origin.y+childSize.y ), childSize ) ;
        case 3: // South east node
            return boundingBox( glm::f32vec2( origin.x+childSize.x, origin.y+childSize.y ), childSize );
        default:
            assert( false && "ERROR <computeBox(...)>: invalid child index!" );
            return boundingBox();
    }
}

int quadTree::getQuadrant(const boundingBox &nodeBox, const boundingBox &valBox) const
{
    auto center = nodeBox.getCenter();
    // checking in which part we are West or East
    // West
    if( valBox.getRight() < center.x )
    {
        //checking North West quadrant
        if( valBox.getBottom() < center.y )
            return 0;
        //checking South West quadrant
        else if( valBox.top >= center.y )
            return 2;
        //nothing was found in both quadrants
        else
            return -1;
    }
    //East
    else if( valBox.left >= center.x )
    {
        //checking North East quadrant
        if( valBox.getBottom() < center.y )
            return 1;
        //checking South East quadrant
        else if( valBox.top >= center.y )
            return 3;
        //nothing was found in both quadrants
        else
            return -1;
    }
    //nothing was found in all quadrants
    else
        return -1;

}

void quadTree::insert(quadTree::qNode *node, size_t depth, const boundingBox &box, const glm::f32vec2 pos, const glm::f32 &val)
{
    assert(node != nullptr);
    assert(box.contains(getBoundingBox(pos)));

    if( isLeaf(node) )
    {
        //insert val in this node if possible
        if( depth >= mDepth || node->values.size() < thres )
            node->values.push_back(glm::f32vec3(val, pos));
        //otherwise split the node and try again
        else
        {
            if( val >= surf_lim.x && val <= surf_lim.y )
            {
                split( node, box );
                insert( node, depth, box, pos, val );
            }
            else if ( val < surf_lim.x && depth < 3)
            {
                split( node, box );
                insert( node, depth, box, pos, val );
            }
            else if( val > surf_lim.y && depth < 3)
            {
                split( node, box );
                insert( node, depth, box, pos, val );
            }
        }
    }
    //otherwise we have a branch and we need to add new node pointers to children
    else
    {
        auto i = getQuadrant( box, getBoundingBox(pos) );
        // Add the value in a child if the value is entirely contained in it
        if( i != -1)
            insert(node->children[static_cast<size_t>(i)].get(), depth+1, computeBox(box, i), pos, val);
        // Otherwise, we add the value in the current node
        else
            node->values.push_back(glm::f32vec3(val, pos));
    }
}

void quadTree::split(quadTree::qNode *node, const boundingBox &box)
{
    //only leafs can be splitted
    assert(node != nullptr);
    assert(isLeaf(node) && "NOTE: Splitting leaves only!");

    //create children
    for( auto& child : node->children )
        child = std::make_unique<qNode>();

    //filling in new values
    auto newValues = std::vector<glm::f32vec3>();

    for( const auto &value : node->values )
    {
        glm::f32vec2 pos(value.y, value.z );
        auto i = getQuadrant( box, getBoundingBox(pos));
        //if the quadrant index found, push down the tree the values of the current node;
        if(i != -1)
            node->children[static_cast<size_t>(i)]->values.push_back(value);
        else
            newValues.push_back(value);
    }
    node->values = std::move(newValues);
}

void quadTree::remove( quadTree::qNode *node, quadTree::qNode *parent,
                       const boundingBox &box, const glm::f32vec2 pos, const glm::f32 &val )
{
    assert( node != nullptr );
    assert( box.contains( getBoundingBox(pos)) );

    if( isLeaf(node) )
    {
        //removing the value from the node
        removeValue( node, pos, val);
        // try to merge the parent
        if( parent != nullptr )
            tryMergeNodes( parent );
    }
    else
    {
        //remove the value in a child if the value entirely ontained in it
        auto i = getQuadrant( box, getBoundingBox(pos));
        if( i != -1 )
            remove(node->children[static_cast<size_t>(i)].get(), node, computeBox(box, i), pos, val);
        else
            //otherwise removing the value from the current node
            removeValue( node, pos, val );
    }
}

void quadTree::removeValue(quadTree::qNode *node, const glm::f32vec2 pos, const glm::f32 &val)
{
    //find the value in node->values
    glm::f32vec3 nodeVal(val, pos);
    auto it = std::find_if( std::begin(node->values), std::end(node->values),
                            [this, &nodeVal](const auto& rhs) { return nodeVal == rhs; });
    assert( it != std::end(node->values) && "error: trying to remove value that is not presented in the node!");
    //swap with the last element and pop back as the order of values is not important
    *it = std::move( node->values.back() );
    node->values.pop_back();
}

void quadTree::tryMergeNodes(quadTree::qNode *node)
{
    assert( node != nullptr );
    assert( !isLeaf(node) && "ERROR: only interior nodes can be merged!");
    auto nbValuesSize = node->values.size();
    for( const auto &child : node->children )
    {
        if( !isLeaf(child.get()) )
            return;
        else
            nbValuesSize += child->values.size();
    }
    if( nbValuesSize <=- thres )
    {
        node->values.reserve(nbValuesSize);
        //merge the values of all children
        for( const auto& child : node->children )
        {
            for( const auto& value : child.get()->values )
                node->values.push_back(value);
        }
        //remove the children
        for( auto& child : node->children )
            child.reset();
    }
}

std::vector<quadTree::qNode *> quadTree::findLeaves(quadTree::qNode *node, boundingBox box)
{
    boundingBox newBox;

    if( isLeaf(node) )
    {
        node->nodeBox = box;
        leaves.push_back(node);
    }
    else
    {
        newBox = computeBox( box, 0 );
        findLeaves(node->children[0].get(), newBox);

        newBox = computeBox( box, 1 );
        findLeaves(node->children[1].get(), newBox);

        newBox = computeBox( box, 2 );
        findLeaves(node->children[2].get(), newBox);

        newBox = computeBox( box, 3 );
        findLeaves(node->children[3].get(), newBox);
    }
    return leaves;
}


}//namespace quadtree
