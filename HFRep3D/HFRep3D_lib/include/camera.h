#ifndef H_CAMERA_CLASS
#define H_CAMERA_CLASS

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

namespace myCamera {

class camera
{
public:
    camera( glm::vec3 startPos, glm::vec3 startUpPos,
            GLfloat startYaw, GLfloat startPitch,
            GLfloat startMoveSpeed, GLfloat startTurnSpeed );
    ~camera() { }

    void keyControl( bool *keys, GLfloat deltaTime );
    void mouseControl( GLfloat xChange, GLfloat yChange );

    inline glm::mat4 calculateViewMatrix() { return glm::lookAt( position, position + frontPos, upPos ); }
    inline glm::vec3 getCameraPosition()   { return position; }

private:
    glm::vec3 position;
    glm::vec3 frontPos, rightPos;
    glm::vec3 upPos, worldUpPos;

    GLfloat yaw, pitch;
    GLfloat moveSpeed, turnSpeed;

    void update();
};

} // namespace myCamera
#endif
