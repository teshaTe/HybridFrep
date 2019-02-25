#include "include/camera.h"

namespace myCamera {

camera::camera( glm::vec3 startPos, glm::vec3 startUpPos, GLfloat startYaw, GLfloat startPitch,
                                                GLfloat startMoveSpeed, GLfloat startTurnSpeed )
{
    position   = startPos;
    worldUpPos = startUpPos;
    yaw        = startYaw;
    pitch      = startPitch;
    frontPos   = glm::vec3( 0.0f, 0.0f, -1.0f );

    moveSpeed  = startMoveSpeed;
    turnSpeed  = startTurnSpeed;

    update();
}

void camera::keyControl(bool *keys, GLfloat deltaTime)
{
    GLfloat velocity = moveSpeed * deltaTime;

    if( keys[GLFW_KEY_W] )
        position += frontPos * velocity;
    if( keys[GLFW_KEY_S] )
        position -= frontPos * velocity;
    if( keys[GLFW_KEY_A] )
        position -= rightPos * velocity;
    if( keys[GLFW_KEY_D] )
        position += rightPos * velocity;
}

void camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw   += xChange;
    pitch += yChange;

    if( pitch > 89.0f )
        pitch = 89.0f;
    if( pitch < -89.0f )
        pitch = -89.0f;

    update();
}

void camera::update()
{
    frontPos.x = cos( glm::radians( yaw )) * cos( glm::radians( pitch ));
    frontPos.y = sin( glm::radians(pitch));
    frontPos.z = sin( glm::radians( yaw )) * cos( glm::radians( pitch ));

    frontPos = glm::normalize( frontPos );
    rightPos = glm::normalize( glm::cross( frontPos, worldUpPos ));
    upPos    = glm::normalize( glm::cross( rightPos, frontPos ));
}


} // namespace myCamera
