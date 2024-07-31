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
     * Pointer to an array of length equal to the number of genes,
     * used to store the per-gene weights.
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
     * Vector of per-cell scores for this gene set.
     * This has length equal to the number of cells.
     */
    std::vector<double> scores;

    /**
     * Vector of weights of length equal to the number of genes in the set.
     * Each entry contains the weight of each successive gene in the gene set.
     * Weights are guaranteed to be non-negative, where larger values indicate a greater contribution to the low-rank approximation.
     */
    std::vector<double> weights;
};

}

#endif
