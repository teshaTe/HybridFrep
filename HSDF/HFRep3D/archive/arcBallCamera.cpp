#include "include/arcBallCamera.h"

namespace myCamera {

arcBallCamera::arcBallCamera(const glm::vec3 startUpPos, const glm::vec3 &center,
                                    GLfloat radius, const glm::mat4& screenToTCS)
{
    tcsCenter = center;
    tcsRadius = radius;
    curScreenToTCS = screenToTCS;

    worldUpPos = startUpPos;
    frontPos   = glm::vec3( 0.0f, 0.0f, -1.0f );
    position   = glm::vec3( 0.0f, 0.0f, radius );

    //glm quaternions format: w, x, y, z
    lastMDown  = glm::vec3( 0.0, 0.0, 0.0 );
    lastMState = glm::vec3( 0.0, 0.0, 0.0 );
    curMDown   = glm::quat( 1.0, 0.0, 0.0, 0.0 );
    curMDrag   = glm::quat( 1.0, 0.0, 0.0, 0.0 );

    frontPos = glm::normalize( frontPos );
    rightPos = glm::normalize( glm::cross( frontPos, worldUpPos ));
    upPos    = glm::normalize( glm::cross( rightPos, frontPos ));
}

void arcBallCamera::beginDrag(const glm::vec2& mouseScreenCoords)
{
    curMDown         = curMState;
    glm::vec4 coords = ( curScreenToTCS * glm::vec4( mouseScreenCoords.x, mouseScreenCoords.y, 0.0f, 1.0 ));
    lastMDown        = glm::vec3( coords.x, coords.y, coords.z );
}

void arcBallCamera::drag(GLfloat scrCoordX, GLfloat scrCoordY)
{
    glm::vec4 coords = ( curScreenToTCS * glm::vec4( scrCoordX, scrCoordY, 0.0f, 1.0 ));
    lastMState       = glm::vec3( coords.x, coords.y, coords.z );
    mapSphereFrom    = mousePosOnSphere( lastMDown );
    mapSphereTo      = mousePosOnSphere( lastMState );

    //constructing a quaternion from two points on the unit sphere
    curMDrag  = createQuatFromUnitSphere( mapSphereFrom, mapSphereTo );
    curMState = curMDrag * curMDown;

    //perform complex conjugate
    glm::quat q = curMState;
    q.x = -q.x; q.y = -q.y;
    q.z = -q.z; q.w =  q.w;

    viewTransform = glm::mat4( q );
}

glm::vec3 arcBallCamera::mousePosOnSphere(const glm::vec3 &mousePos)
{
    glm::vec3 ballMouse;
    ballMouse.x = ( mousePos.x - tcsCenter.x ) / tcsRadius;
    ballMouse.y = ( mousePos.y - tcsCenter.y ) / tcsRadius;

    GLfloat mag = glm::dot( ballMouse, ballMouse );
    if( mag > 1.0f )
    {
        ballMouse  *= 1.0f/glm::sqrt( mag );
        ballMouse.z = 0.0f;
    }
    else {
        ballMouse.z = glm::sqrt( 1.0f - mag );
    }

    return ballMouse;
}

glm::quat arcBallCamera::createQuatFromUnitSphere(const glm::vec3 &fromP, const glm::vec3 &toP)
{
    glm::quat q;
    q.x = fromP.y * toP.z - fromP.z * toP.y;
    q.y = fromP.z * toP.x - fromP.x * toP.z;
    q.z = fromP.x * toP.y - fromP.y * toP.x;
    q.w = fromP.x * toP.x + fromP.y * toP.y + fromP.z * toP.z;
    return q;
}

}//namespace myCamera
