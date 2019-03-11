#include "include/window.h"
#include <iostream>

GLFW_window::window::window(GLint windowWidth, GLint windowHeight)
{
    width  = windowWidth;
    height = windowHeight;

    for (size_t i = 0; i < 1024; i++)
        keys[i] = 0;
}

GLFW_window::window::~window()
{
    glfwDestroyWindow( mainWindow );
    glfwTerminate();
}

bool GLFW_window::window::checkError(int number)
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

bool GLFW_window::window::initialise()
{
    if( !glfwInit() )
    {
        std::cerr << "ERROR: GLFW initialization failled!" <<std::endl;
        glfwTerminate();
        return false;
    }

    //chack + set OpenGL version to 3.3; check for the depricated code + allow forward compatibility
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

    mainWindow = glfwCreateWindow( width, height, "mainWindow", nullptr, nullptr );
    if( !mainWindow )
    {
        std::cerr << "ERROR: GLFE window creation failled!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwGetFramebufferSize( mainWindow, &bufferWidth, &bufferHeight );
    glfwMakeContextCurrent( mainWindow );

    createCallbacks();
    glfwSetInputMode( mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
    glewExperimental = GL_TRUE;

    GLenum error = glewInit();
    if( error != GLEW_OK )
    {
        std::cerr << "ERROR: " << glewGetErrorString( error ) << std::endl;
        glfwDestroyWindow( mainWindow );
        glfwTerminate();
        return false;
    }

    /*if(GL_TRUE != glewGetExtension("GL_TEXTURE_3D"))
    {
        std::cerr << "\n3D texture is not supported !\n" << std::endl;
    }
    else {
        glEnable(GL_TEXTURE_3D);
    }*/

    glewExperimental = GL_TRUE;
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_3D );

    glViewport( 0, 0, bufferWidth, bufferHeight );
    glfwSetWindowUserPointer( mainWindow, this );

    return true;
}

void GLFW_window::window::createCallbacks()
{
    glfwSetKeyCallback( mainWindow, handleKeys );
    glfwSetCursorPosCallback( mainWindow, handleMouse );
}

void GLFW_window::window::handleKeys(GLFWwindow *curWindow, int key, int code, int action, int mode)
{
    window* theWindow = static_cast<window*>( glfwGetWindowUserPointer( curWindow ));

    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
        glfwSetWindowShouldClose( curWindow, GL_TRUE );

    if( key >= 0 && key < 1024 )
    {
        if( action == GLFW_PRESS )
            theWindow->keys[key] = true;
        else if ( action == GLFW_RELEASE )
            theWindow->keys[key] = false;
    }
}

void GLFW_window::window::handleMouse(GLFWwindow *curWindow, double xPos, double yPos)
{
    window* theWindow = static_cast<window*>( glfwGetWindowUserPointer( curWindow ) );

    if( theWindow->mouseFirstMoved )
    {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mouseFirstMoved = false;
    }

    theWindow->xChange = xPos - theWindow->lastX;
    theWindow->yChange = theWindow->lastY - yPos;
    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
}

GLfloat GLFW_window::window::getXchange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;
}

GLfloat GLFW_window::window::getYchange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;
}
