#ifndef H_SFML_WINDOW_CLASS
#define H_SFML_WINDOW_CLASS

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <memory>

#include "shader.h"

namespace sfml_window {

class window
{
public:
    window(GLint width, GLint height);
    ~window(){}

    bool createWindow(sf::String name);
    void keyboardContr();
    void mouseContr();
    void saveImage(sf::Window *curWin);
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
    GLuint uniformMousePos;

    GLfloat lastX, lastY;     // last coords of the mouse pos
    GLfloat xChange, yChange; // how much the position of the mouse was changed comparing to the previous one

    bool keys[15];
    int ind;

    std::shared_ptr<hfrep3D::shader> curShader;
};

} //namespace sfml_window
#endif // H_SFML_WINDOW_CLASS
