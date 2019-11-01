/***
 * This code is based on the tutorial https://pvigier.github.io/2019/08/04/quadtree-collision-detection.html
 * by pierre
 */

#ifndef H_QUADTREE2_CLASS
#define H_QUADTREE2_CLASS

#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <memory>

namespace hfrep2D {

class boundingBox
{
public:
    constexpr boundingBox( glm::f32 Left = 0, glm::f32 Top = 0, glm::f32 Width = 0, glm::f32 Height=0 ) noexcept:
                           left(Left), top(Top), width(Width), height(Height), val_NE(0), val_NW(0), val_SE(0), val_SW(0) { }
    constexpr boundingBox( const glm::f32vec2 pos, const glm::f32vec2 size ) noexcept:
                           left(pos.x), top(pos.y), width(size.x), height(size.y),val_NE(0), val_NW(0), val_SE(0), val_SW(0) { }

    constexpr glm::f32 getRight()          const noexcept { return left + width; }
    constexpr glm::f32 getBottom()         const noexcept { return top + height; }
    constexpr glm::f32vec2 getTopLeft()    const noexcept { return glm::f32vec2( left, top ); }
    constexpr glm::f32vec2 getCenter()     const noexcept { return glm::f32vec2( left+width/2, top+height/2 ); }
    constexpr glm::f32vec2 getSize()       const noexcept { return glm::f32vec2( width, height ); }
    constexpr glm::f32vec4 getCornerVals() const noexcept { return glm::vec4( val_NW, val_NE, val_SW, val_SE ); }

    constexpr bool contains( const boundingBox &box ) const noexcept
    {
        return left <= box.left && box.getRight() <= getRight() &&
                top <= box.top && box.getBottom() <= getBottom();
    }

    friend class quadTree;
    friend class quadTreeRender;

private:
    glm::f32 left, top;
    glm::f32 width, height;
    glm::f32 val_NE, val_NW, val_SE, val_SW;
};

class quadTree
{
public:
    struct qNode
    {
        std::array<std::unique_ptr<qNode>, 4> children;
        //values are the grid points that are in the current node
        std::vector<glm::f32vec3> values; // values.x = value; values.yz - coordinates of the point containing the value
        boundingBox nodeBox;
    };
    inline bool isLeaf(qNode *node) const { return !static_cast<bool>(node->children[0]); }

public:
    quadTree( const boundingBox &box, const int resX, const int resY, const size_t threshold, const size_t maxDepth):
              mRoot(std::make_unique<qNode>()), mBox(box), thres(threshold), mDepth(maxDepth), res_x(resX), res_y(resY) { }

    quadTree(){}

    ~quadTree() { }

    void generateQuadTree(const std::vector<float> *inField, const glm::vec2 surfLim);
    void deleteQuadTree(qNode *node);

    inline void insertNode( const glm::f32vec2 pos, const glm::f32 &value ) { insert( mRoot.get(), 0, mBox, pos, value ); }
    inline void removeNode( const glm::f32vec2 pos, const glm::f32 &value ) { remove( mRoot.get(), nullptr, mBox, pos, value ); }
    inline std::vector<qNode*> getLeaves() { return findLeaves( mRoot.get(), mBox); }
    inline qNode* getRoot() { return mRoot.get();}

    friend class quadTreeRender;
private:
    /**
     * @brief computeBox - function for computing new bounding boxes on the fly
     *                     it computes the box of a child from the 'box' of the parent and index 'i' of the quadrant
     * @param box - bounding box that will be further subdivided
     * @param i   - 0,1,2,3 - the quadrant index
     * @return - returns the subdivided bounding box
     */
    boundingBox computeBox( const boundingBox &box, int i ) const;

    boundingBox getBoundingBox( glm::f32vec2 pos ) { return boundingBox(pos, glm::f32vec2(0, 0)); }

    /**
     * @brief getQuadrant - checking if the passed bounding box is fully contained in any quadrant
     * @param nodeBox - is the passed parent box inside of which we are trying to find valBox
     * @param valBox  - is an input bounding box that we are trying to find in one of the quadrants
     * @return - the index of the quadrant or -1 if passed bounding box is not entirely contained in any quadrant
     */
    int getQuadrant( const boundingBox &nodeBox, const boundingBox &valBox ) const;

    /**
     * @brief insert - function that insert new node to the tree
     * @param node  - node to be inserted in the tree
     * @param depth - the depth of the tree at which the node will be inserted
     * @param box   - bounding box
     * @param pos   - position of the point in which value is defined
     * @param val   - value defined in the point with coords <pos>
     */
    void insert( qNode* node, size_t depth, const boundingBox &box, const glm::f32vec2 pos, const glm::f32 &val );

    /**
     * @brief split
     * @param node
     * @param box
     */
    void split( qNode *node, const boundingBox &box );

    /**
     * @brief remove
     * @param node
     * @param parent
     * @param box
     * @param pos
     * @param val
     */
    void remove( qNode *node, qNode *parent, const boundingBox &box, const glm::f32vec2 pos, const glm::f32 &val );

    /**
     * @brief removeValue
     * @param node
     * @param pos
     * @param val
     */
    void removeValue( qNode *node, const glm::f32vec2 pos, const glm::f32 &val );

    /**
     * @brief tryMergeNodes
     * @param node
     */
    void tryMergeNodes(qNode *node);

    std::vector<qNode *> findLeaves(qNode *node , boundingBox box);

private:
    std::unique_ptr<qNode> mRoot;
    std::vector<qNode*> leaves;
    boundingBox mBox;
    size_t thres, mDepth;
    int res_x, res_y;
    glm::f32vec2 surf_lim;
};

} // namespace quadtree
#endif //define H_QUADTREE2_CLASS
