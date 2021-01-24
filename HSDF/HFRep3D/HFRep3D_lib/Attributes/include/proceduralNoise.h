#ifndef H_PROCEDURE_NOISE_CLASS
#define H_PROCEDURE_NOISE_CLASS

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace material {

typedef std::pair<std::vector<float>, std::vector<float>> HObj_single_mat;

struct colRegion
{
    glm::bvec3 activeNoise    = glm::bvec3(true, false, false);
    glm::bvec3 activeDistFunc = glm::bvec3(false, true, false);
    glm::vec3 manualCol       = glm::vec3(0);
    glm::vec4 region;
};

class procNoise
{
public:
    //possible seed value is 2016 and possible permTableSize can be 256;
    procNoise(const int resX, const int resY, const int resZ, const int seed, const unsigned permTableSize);
    ~procNoise() {}

    void procWhiteNoise  (const std::vector<float> fun, HObj_single_mat &obj);
    void procValueNoise  (const std::vector<float> fun, float frequency, HObj_single_mat &obj);
    void procWood        (const std::vector<float> fun, float frequency, HObj_single_mat &obj);
    void procMarble      (const std::vector<float> fun, float frequency, float freqMult,
                          float ampMult, float numLayers, HObj_single_mat &obj);
    void procFractalNoise(const std::vector<float> fun, float frequency, float freqMult,
                          float ampMult, float numLayers, float maxNoiseV, HObj_single_mat &obj);
    void procTurbulence  (const std::vector<float> fun, float frequency, float freqMult,
                          float ampMult, float numLayers, float maxNoiseV, HObj_single_mat &obj);

private:
    float eval(glm::vec3 &p);
    void valueNoiseInit();

    inline float scaleToNewRange(float v, float oldMin, float oldMax, float newMin=0, float newMax = 1.0)
                                { return ((v - oldMin)*(newMax - newMin)/(oldMax - oldMin)) + newMin; }
    inline float lerp(const float &lo, const float &hi, const float &t) { return lo*(1.0f - t) + hi*t; }
    inline float smothstep(const float &t) { return t * t * (3.0f - 2.0f*t); }
    inline int index3d(int x, int y, int z) { return z*res_x*res_y + y*res_x + x; }

private:
    int res_x, res_y, res_z, randSeed;
    unsigned maxTableSizeMask;
    unsigned tableSize;

    std::vector<unsigned> permutationTable;
    std::vector<float> r;
};

} // namespace material
#endif //H_PROCEDURE_NOISE_CLASS
