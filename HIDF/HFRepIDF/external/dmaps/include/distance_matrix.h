#ifndef H_DIST_MATRIX_CLASS
#define H_DIST_MATRIX_CLASS

#include <functional>
#include <string>
#include "types.h"

namespace dmaps {

class distance_matrix
{
private:
    // Distance matrix.
    matrix_t d_;

    // Weights vector.
    vector_t w_;

    // Coordinates.
    matrix_t x_;

public:
    distance_matrix(const matrix_t& x, int num_threads = 0);
    distance_matrix(const matrix_t& x, const vector_t& w, int num_threads = 0);
    distance_matrix(const std::string& filename, int num_threads = 0);

    const matrix_t& get_coordinates() const;
    const matrix_t& get_distances() const;

    void compute(const std::function<f_type(const vector_t&, const vector_t&, const vector_t&)>&);
    vector_t compute(const vector_t& coor, const std::function<f_type(const vector_t&, const vector_t&, const vector_t&)>&);
    void save(const std::string& filename) const;
};

} // namespace dmaps
#endif
