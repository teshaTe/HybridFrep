#ifndef H_ARCBALL_CAMERA_CLASS
#define H_ARCBALL_CAMERA_CLASS

/*This implemetationis based on the code
 * https://github.com/iauns/cpm-arc-ball/blob/master/arc-ball/
*/
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace myCamera {

class arcBallCamera {
public:
    /* center - center of the acrBall camera in screen coordinated if viewMatrix is identity matrix
     *          otherwise it is set in target coordinate system;
     * radius - radius in target coordinate system;
     * viewMatrix - transformation from screen coordinate system to target coordinate system;
     */
    arcBallCamera(const glm::vec3 startUpPos,const glm::vec3& center, GLfloat radius, const glm::mat4 &screenToTCS=glm::mat4(1));
    ~arcBallCamera() { }

    void beginDrag( const glm::vec2& mouseScreenCoords );
    void drag( GLfloat scrCoordX, GLfloat scrCoordY );

    inline glm::mat4 calculateViewMatrix()     { return glm::lookAt( position, position + frontPos, upPos ); }
    inline glm::mat4 getTransformationMatrix() { return viewTransform; }

private:
    //caclulating current position on the arcball from 2D mouse position
    glm::vec3 mousePosOnSphere( const glm::vec3& mousePos );
    //construct a unit quaternion from two points on the unit sphere
    glm::quat createQuatFromUnitSphere( const glm::vec3& fromP, const glm::vec3& toP );

    glm::vec3 tcsCenter;      // center of the arcball in target coordinate system (tcs)
    GLfloat   tcsRadius;      // radius of the arcball in tcs

    glm::quat curMState;      // current state of the rotation taking into account mouse
    glm::quat curMDown;       // state of the rotation since mouse was moved down
    glm::quat curMDrag;       // dragged transform of the mouse

    glm::vec3 lastMState;     // most current tcs position of the mouse during drag
    glm::vec3 lastMDown;      // tcs posiiton of the mouse when the drag was begun
    glm::vec3 mapSphereFrom;  // lastMDown mapped to the sphere of tscRadius centered at tscCenter in tsc
    glm::vec3 mapSphereTo;    // lastMState mapped to the sphere of tscRadius centered at tscCenter in tsc

    glm::mat4 viewTransform;  // matrix representing the current rotation of the camera
    glm::mat4 curScreenToTCS;

    glm::vec3 upPos, worldUpPos, position, frontPos, rightPos;
};

} // namespace myCamera
#endif
