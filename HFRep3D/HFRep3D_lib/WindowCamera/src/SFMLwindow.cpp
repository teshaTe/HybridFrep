#include "SFMLwindow.h"
#include <iostream>

namespace sfml_window {

window::window(GLint width, GLint height)
{
    bufferWidth = width;
    bufferHeight = height;

    lastX = bufferWidth/2;
    lastY = bufferHeight/2;

    for (size_t i = 0; i < 4; i++)
        keys[i] = 0;
}

bool window::createWindow(sf::String name)
{
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 0;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    nWindow.create(sf::VideoMode(bufferWidth, bufferHeight), name, sf::Style::Default, settings);
    nWindow.setFramerateLimit(60);

    if(!nWindow.isOpen())
    {
        std::cerr <<"ERROR: something went wrong while creating window!" << std::endl;
        return false;
    }

    nWindow.setActive(true);

    nWindow.setMouseCursorGrabbed(true);
    sf::Mouse::setPosition(sf::Vector2i(bufferWidth/2, bufferHeight/2), nWindow);
    nWindow.setKeyRepeatEnabled(true);

    GLenum error = glewInit();
    if( error != GLEW_OK )
    {
        std::cerr << "ERROR: " << glewGetErrorString( error ) << std::endl;
        nWindow.close();
        return false;
    }
    glewExperimental = GL_TRUE;
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_3D );
    glEnable( GL_TEXTURE_2D );
    glViewport( 0, 0, bufferWidth, bufferHeight );

    return true;
}

void window::keyboardContr()
{
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) )
    {
        nWindow.close();
        exit(0);
    }

    if( sf::Keyboard::isKeyPressed(sf::Keyboard::W) )
        keys[0] = true;
    else
        keys[0] = false;

    if( sf::Keyboard::isKeyPressed(sf::Keyboard::S) )
        keys[1] = true;
    else
        keys[1] = false;

    if( sf::Keyboard::isKeyPressed(sf::Keyboard::A) )
        keys[2] = true;
    else
        keys[2] = false;

    if( sf::Keyboard::isKeyPressed(sf::Keyboard::D) )
        keys[3] = true;
    else
        keys[3] = false;

}

void window::mouseContr()
{
    xChange = sf::Mouse::getPosition().x - lastX;
    yChange = lastY - sf::Mouse::getPosition().y;
    lastX = sf::Mouse::getPosition().x;
    lastY = sf::Mouse::getPosition().y;
}

bool window::checkError(int number)
{
    GLenum errCode = glGetError();
    const GLubyte *errString;
    if((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        std::cerr << "\nERROR < "<< number << " > :" << errString << std::endl;
        return false;
    }
    return true;
}

GLfloat window::getXchange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;
}

GLfloat window::getYchange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;
}




} //namespace sfml_window
