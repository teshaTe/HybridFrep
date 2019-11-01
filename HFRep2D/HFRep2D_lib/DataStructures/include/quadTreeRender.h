#ifndef QUAD_TREE_RENDER_H
#define QUAD_TREE_RENDER_H

#include <SFML/Graphics/RenderWindow.hpp>
#include "quadTree.h"

namespace hfrep2D
{

class quadTreeRender
{
public:
    quadTreeRender(){}
    ~quadTreeRender(){}

    void createWindow( int resX, int resY, std::string name);
    inline void displayTree( quadTree *tree, std::string fileName="" ) { display(tree->getLeaves(), fileName); }

private:
    void display(std::vector<quadTree::qNode*> leaves , std::string fileName);
    void drawRectangle( float x, float y, float width, float height, sf::RenderTarget &canvas );

    sf::Image saveTreeImg(std::string fileName);

private:
    int winResX, winResY;
    sf::RenderWindow *newWindow;

};

} // namespace quadtree
#endif // QUAD_TREE_RENDER_H
