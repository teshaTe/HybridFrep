#ifndef H_INTERPOLATION_2D_CLASS
#define H_INTERPOLATION_2D_CLASS

#include "diplib.h"
#include <vector>
#include <glm/glm.hpp>

namespace hfrep2D {

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
    GEO_AKIMA,
    GEO_BICUBIC,
    GEO_BILINEAR,
    GEO_BSPLINE,
    GEO_THINPLATE
};

class inter2D
{
public:
    inter2D() {}
    ~inter2D() {}

    std::vector<float> dipLibResample2D(std::vector<float> *field, float zoom, InterType type, glm::ivec2 res);
    std::vector<float> geoToolsResample2D(std::vector<float> *field, InterType type, glm::ivec2 inRes,
                                           glm::ivec2 outRes, glm::vec2 step, bool precise=true);
};

} //namespace hfrep2D
#endif
