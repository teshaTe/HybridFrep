#ifndef H_FIELD_OPERATIONS

#include <glm/glm.hpp>
#include <vector>
#include <functional>

namespace frep {
class FieldOperations
{
public:
    FieldOperations() = default;

    float spaceTimeBlending(float f1, float f2, float a0, float a1, float a2, float a3, float time, float setTheor_c1, float setTheor_c2,
                            std::function<float (float, float, float, float)> func);
    std::vector<float> constOffset(const std::vector<float> field, const float offset);

    float union_R0(float f1, float f2, float n, float dummy = 0);
    float intersect_R0(float f1, float f2, float n, float dummy = 0);
    float subtract_R0(float f1, float f2, float n, float dummy = 0);

    float union_R1(float f1, float f2, float alpha = 0, float m = 0);
    float intersect_R1(float f1, float f2, float alpha = 0, float m = 0);
    float subtract_R1(float f1, float f2, float alpha = 0, float m = 0);

    std::vector<float> scaleFunction(const std::vector<float> field, const float factor);
    glm::vec2 findZeroLevelSetInterval(const std::vector<float> field, const int numElemsToAverage);

    ~FieldOperations() = default;
private:

};



} //namespace frep
#endif //H_FIELD_OPERATIONS
