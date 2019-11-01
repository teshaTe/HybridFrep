#include "quadTreeRender.h"
#include <SFML/Graphics.hpp>

namespace hfrep2D
{

void quadTreeRender::createWindow(int resX, int resY, std::string name)
{
    newWindow = new sf::RenderWindow(sf::VideoMode( resX, resY), name, sf::Style::Default );
    newWindow->setFramerateLimit(60);
}

sf::Image quadTreeRender::saveTreeImg(std::string fileName)
{
    sf::Texture texture;
    texture.create(newWindow->getSize().x, newWindow->getSize().y );
    texture.update(*newWindow);
    sf::Image screenshot = texture.copyToImage();
    screenshot.saveToFile( fileName );

    return screenshot;
}

void quadTreeRender::display(std::vector<quadTree::qNode*> leaves, std::string fileName)
{
    while( newWindow->isOpen() )
    {
        sf::Event event;
        while( newWindow->pollEvent(event) )
        {
            if( event.type == sf::Event::Closed )
            {
                if(!fileName.empty())
                    saveTreeImg(fileName);
                newWindow->close();
            }
            else if(event.type == sf::Event::KeyPressed )
            {
                if(event.key.code == sf::Keyboard::Escape)
                {
                    if(!fileName.empty())
                        saveTreeImg(fileName);
                    newWindow->close();
                }
            }
        }
        newWindow->clear(sf::Color::White);

        for(size_t i = 0; i < leaves.size(); i++)
        {
            drawRectangle( leaves[i]->nodeBox.left,  leaves[i]->nodeBox.top,
                           leaves[i]->nodeBox.width, leaves[i]->nodeBox.height, *newWindow );
        }

        newWindow->display();
    }
}

void quadTreeRender::drawRectangle(float x, float y, float width, float height, sf::RenderTarget &canvas)
{
    sf::RectangleShape shape;
    shape.setPosition(x, y);
    shape.setSize( sf::Vector2f( width, height ));
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor( sf::Color( 0, 0, 0 ));
    shape.setFillColor( sf::Color( 255, 255, 255, 255 ));
    canvas.draw(shape);
}

} //namespace quadtree
