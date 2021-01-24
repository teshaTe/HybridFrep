#include "include/OperationsOverFields.h"
#include <algorithm>

namespace frep {

float FieldOperations::spaceTimeBlending(float f1, float f2, float a0, float a1, float a2, float a3,
                                         float time, float setTheor_c1, float setTheor_c2,
                                         std::function<float (float, float, float, float)> func)
{
    f1 = func(f1, -time, setTheor_c1, setTheor_c2);
    f2 = func(f2, time - 1.0f, setTheor_c1, setTheor_c2);
    float f3 = func(time + 10.0f, 10.0f - time, setTheor_c1, setTheor_c2);

    float r1 = (f1/a1)*(f1/a1) + (f2/a2)*(f2/a2);
    float r2 = 0.0f;

    if(f3 > 0.0f)
      r2 = (f3/a3) * (f3/a3);

    float rr = 0.0f;
    if(r1 > 0.0f)
      rr = r1 / (r1 + r2);

    float d = 0.0f;
    if(rr < 1.0f)
      d = a0 * (1.0f - rr)*(1.0f - rr)*(1.0f - rr) / (1.0f + rr);

    return func(f1, f2, setTheor_c1, setTheor_c2) + d;
}

std::vector<float> FieldOperations::constOffset(const std::vector<float> field, const float offset)
{
    std::vector<float> offsetedField;
    std::transform(field.begin(), field.end(), std::back_inserter(offsetedField),
                   std::bind(std::plus<float>(), offset, std::placeholders::_1));
    return offsetedField;

}

float FieldOperations::union_R0(float f1, float f2, float n, float dummy)
{
    float result = 0.0f;

    if(f1 > 0.0f && f2 > 0.0f)
       result = std::pow(std::pow(f1, n) + std::pow(f2, n), 1.0f / n);
    else if(f1 <= 0.0f && f2 >= 0.0f)
       result = f2;
    else if(f1 >= 0.0f && f2 <= 0.0f)
       result = f1;
    else if(f1 < 0.0f && f2 < 0.0f)
       result = std::pow(-1.0f, n + 1.0f) * f1 * f2 * std::pow(std::pow(f1, n) + std::pow(f2, n), -1.0f / n);

    return result;
}

float FieldOperations::intersect_R0(float f1, float f2, float n, float dummy)
{
    float result = 0.0f;

    if(f1 > 0.0f && f2 > 0.0f)
       result = f1 * f2 * std::pow(std::pow(f1, n) + std::pow(f2, n), -1.0f / n);
    else if(f1 <= 0.0f && f2 >= 0.0f)
       result = f1;
    else if(f1 >= 0.0f && f2 <= 0.0f)
       result = f2;
    else if(f1 < 0.0f && f2 < 0.0f)
       result = std::pow(-1.0f, n + 1.0f) * std::pow(std::pow(f1, n) + std::pow(f2, n), 1.0f / n);

    return result;
}

float FieldOperations::subtract_R0(float f1, float f2, float n, float dummy)
{
    return intersect_R0(f1, -f2, n);
}

float FieldOperations::union_R1(float f1, float f2, float alpha, float m)
{
    return (1.0f/(1.0f+alpha))*(f1 + f2 + std::sqrt(f1 * f1 + f2 * f2 - 2*alpha*f1*f2)*std::pow(f1*f1+f2*f2, m/2.0f));
}

float FieldOperations::intersect_R1(float f1, float f2, float alpha, float m)
{
    return (1.0f/(1.0f+alpha))*(f1 + f2 - std::sqrt(f1 * f1 + f2 * f2 - 2*alpha*f1*f2)*std::pow(f1*f1+f2*f2, m/2.0f));
}

float FieldOperations::subtract_R1(float f1, float f2, float alpha, float m)
{
    return intersect_R1(f1, -f2, alpha, m);
}

std::vector<float> FieldOperations::scaleFunction(const std::vector<float> field, const float factor)
{
    std::vector<float> scaledField;
    std::transform(field.begin(), field.end(), std::back_inserter(scaledField),
                   std::bind(std::multiplies<float>(), factor, std::placeholders::_1));
    return scaledField;
}

glm::vec2 FieldOperations::findZeroLevelSetInterval(const std::vector<float> field, const int numElemsToAverage)
{
    std::vector<float> posVals, negVals;

    for(size_t i = 0; i < field.size(); i++)
    {
        if(field[i] >= 0)
            posVals.push_back(field[i]);
    }

    std::vector<float> minPosV;
    for(int j = 0; j < numElemsToAverage; j++)
    {
        std::vector<float>::iterator minPos = std::min_element(posVals.begin(), posVals.end());
        minPosV.push_back(*minPos);
        posVals.erase(std::remove(posVals.begin(), posVals.end(), *minPos), posVals.end());
    }

    float minPos, minPsum = 0;
    for(size_t i = 0; i < minPosV.size(); i++)
        minPsum += minPosV[i];

    minPos = minPsum / static_cast<float>(minPosV.size());

    glm::vec2 result = glm::vec2(0.0f, minPos);
    return result;
}

} // namespace frep

