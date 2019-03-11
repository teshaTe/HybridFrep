#ifndef H_WINDOW_CLASS
#define H_WINDOW_CLASS

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace GLFW_window {

class window
{
public:
    window( GLint windowWidth, GLint windowHeight );
    ~window();

    bool initialise();

    GLfloat getXchange();
    GLfloat getYchange();

    bool checkError(int number);

    inline GLfloat getBufferWidth()  { return bufferWidth; }
    inline GLfloat getBufferHeight() { return bufferHeight; }

    inline bool* getKeys()       { return keys; }
    inline bool getShouldClose() { return glfwWindowShouldClose( mainWindow ); }
    inline void swapBuffers()    { glfwSwapBuffers( mainWindow ); }

private:
    GLFWwindow *mainWindow;
    GLint   width, height;
    GLsizei bufferWidth, bufferHeight;

    bool keys[1024];

    GLfloat lastX, lastY;     // last coords of the mouse pos
    GLfloat xChange, yChange; // how much the position of the mouse was changed comparing to the previous one

    bool mouseFirstMoved;

    //keyboatd and mouase input handling
    static void handleKeys ( GLFWwindow *curWindow, int key, int code, int action, int mode );
    static void handleMouse(GLFWwindow *curWindow, double xPos, double yPos );

    void createCallbacks();

};

}//namespace GLFW_window

#endif
