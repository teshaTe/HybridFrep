#ifndef H_SFML_WINDOW_CLASS
#define H_SFML_WINDOW_CLASS

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

namespace sfml_window {

class window
{
public:
    window(GLint width, GLint height);
    ~window(){}

    bool createWindow(sf::String name);
    void keyboardContr();
    void mouseContr();

    bool checkError(int number);

    GLfloat getXchange();
    GLfloat getYchange();

    inline bool* getKeys() { return keys; }
    inline sf::Window *getWindow() { return &nWindow; }
    inline GLfloat getBufferWidth()  { return bufferWidth; }
    inline GLfloat getBufferHeight() { return bufferHeight; }

private:
    GLint   width, height;
    GLsizei bufferWidth, bufferHeight;
    sf::Window nWindow;

    GLfloat lastX, lastY;     // last coords of the mouse pos
    GLfloat xChange, yChange; // how much the position of the mouse was changed comparing to the previous one

    bool keys[4];
};

} //namespace sfml_window
#endif // H_SFML_WINDOW_CLASS
