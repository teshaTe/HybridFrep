#include "quadTree.h"
#include "quadTreeRender.h"
#include "frep2D.h"

#include <SFML/Graphics/Image.hpp>

#include <iostream>

int main()
{
    int resX = 512, resY = 512;
    float surf_l = -0.0005f, surf_r = 0.0005f;
    float rad = 150.0f;
    size_t max_depth = 8, thres = 16;

    hfrep2D::FRepObj2D frep( resX, resY, 1.0f );
    auto key = std::bind(&hfrep2D::FRepObj2D::trebleClef, frep, std::placeholders::_1 );

    std::vector<float> func = frep.getFRep2D( key );

    hfrep2D::boundingBox box( glm::f32vec2(0,0), glm::f32vec2(resX, resY) );
    hfrep2D::quadTree tree( box, resX, resY, thres, max_depth );
    tree.generateQuadTree( &func, glm::vec2( surf_l, surf_r ));

    std::vector<hfrep2D::quadTree::qNode*> leaves = tree.getLeaves();

    for(size_t i = 0; i < leaves.size(); i++ )
    {
        if( leaves[i]->nodeBox.getSize().x == 0.0f || leaves[i]->nodeBox.getSize().y == 0.0f )
        {
            std::cout << "this node has empty bbox!: [" << i <<"]"
                      <<"  x = " << leaves[i]->nodeBox.getSize().x
                      <<"  y = " << leaves[i]->nodeBox.getSize().y << std::endl;
        }
    }

    hfrep2D::quadTreeRender render;
    render.createWindow(resX, resY, "Test");
    render.displayTree( &tree, "qtree.jpg" );

    return 0;
}
