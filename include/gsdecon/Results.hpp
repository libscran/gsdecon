#ifndef GSDECON_RESULTS_HPP
#define GSDECON_RESULTS_HPP

#include <vector>

/**
 * @file Results.hpp
 * @brief Classes for storing the results.
 */

namespace gsdecon {

/**
 * @brief Buffers for the `compute()` results.
 * @tparam Float_ Floating-point type for the results.
 */
template<typename Float_>
struct Buffers {
    /**
     * Pointer to an array of length equal to the number of cells,
     * used to store the per-cell scores.
     */
    Float_* scores;

    /**
     * Pointer to an array of length equal to the number of features,
     * used to store the per-feature weights.
     */
    Float_* weights;
};

/**
 * @brief Results of `compute()`.
 * @tparam Float_ Floating-point type for the results.
 */
template<typename Float_>
struct Results {
    /**
     * Vector of per-cell scores for this feature set.
     * This has length equal to the number of scores in the dataset.
     */
    std::vector<double> scores;

    /**
     * Vector of weights of length equal to the number of features in the set.
     * Each entry contains the weight of each successive feature in the feature set.
     * Weights may be negative.
     */
    std::vector<double> weights;
};

}

#endif
