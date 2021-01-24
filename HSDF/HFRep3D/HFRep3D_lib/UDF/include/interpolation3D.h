#ifndef H_INTERPOLATION_3D_CLASS
#define H_INTERPOLATION_3D_CLASS

#include "diplib.h"
#include <vector>
#include <glm/glm.hpp>

namespace hfrep3D {

enum class InterType
{
    DIP_CUBIC_THIRD_ORDER,
    DIP_CUBIC_FOURTH_ORDER,
    DIP_LINEAR,
    DIP_NEAREST,
    DIP_BSPLINE,
    DIP_LANCZOS2,
    DIP_LANCZOS3,
    DIP_LANCZOS4,
    DIP_LANCZOS6,
    DIP_LANCZOS8,
    GEO_TRICUBIC,
    GEO_AKIMA,
    GEO_BSPLINE,
    GEO_TRILINEAR
};

class inter3D
{
public:
    inter3D() {}
    ~inter3D() {}

    std::vector<float> dipLibResample3D(std::vector<float> *field, float zoom,
                                        InterType type, glm::ivec3 res);
    std::vector<float> geoToolsResample3D(std::vector<float> *field, InterType type, glm::ivec3 inRes,
                                          glm::ivec3 outRes, glm::vec3 step, bool precise=true);
private:
    inline int index3d( int x, int y, int z, glm::vec3 res ) { return z*res.x*res.y + y*res.x + x; }
};

} //namespace hfrep2D

#endif
