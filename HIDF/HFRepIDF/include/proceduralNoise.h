#ifndef H_PROCEDURE_NOISE_CLASS
#define H_PROCEDURE_NOISE_CLASS

#include <Eigen/Core>
#include <vector>

namespace material {

class procNoise
{
public:
    procNoise(const int seed, const unsigned permTableSize);
    ~procNoise() {}

    //void procWhiteNoise  (const std::vector<float> fun, std::pair<std::vector<float>, std::vector<float>> &obj);
    //void procValueNoise  (const std::vector<float> fun, float frequency, std::pair<std::vector<float>, std::vector<float>> &obj);
    void procWood        (const Eigen::VectorXd field, const Eigen::MatrixXd V, float frequency, Eigen::MatrixXd &texture);
    void procMarble      (const Eigen::VectorXd field, const Eigen::MatrixXd V, float frequency, float freqMult,
                          float ampMult, float numLayers, Eigen::MatrixXd &texture);
    //void procFractalNoise(const std::vector<float> fun, float frequency, float freqMult,
    //                      float ampMult, float numLayers, float maxNoiseV, std::pair<std::vector<float>, std::vector<float>> &obj);
    //void procTurbulence  (const std::vector<float> fun, float frequency, float freqMult,
    //                      float ampMult, float numLayers, float maxNoiseV, std::pair<std::vector<float>, std::vector<float>> &obj);


private:
    float eval(Eigen::Vector2d &p);
    void valueNoiseInit();

    inline float lerp(const float &lo, const float &hi, const float &t) { return lo*(1.0f - t) + hi*t; }
    inline float smothstep(const float &t) { return t * t * (3.0f - 2.0f*t); }

private:
    int randSeed;
    unsigned maxTableSizeMask;
    unsigned tableSize;

    std::vector<unsigned> permutationTable;
    std::vector<float> r;
};

} // namespace material
#endif //H_PROCEDURE_NOISE_CLASS
