#ifndef GSDECON_COMPUTE_HPP
#define GSDECON_COMPUTE_HPP

#include <algorithm>
#include <vector>

#include "tatami/tatami.hpp"
#include "irlba/irlba.hpp"
#include "scran_pca/scran_pca.hpp"

#include "Options.hpp"
#include "Results.hpp"
#include "utils.hpp"

/**
 * @file compute.hpp
 * @brief Compute per-cell scores for a feature set.
 */

namespace gsdecon {

/**
 * Per-cell scores are defined as the column means of the low-rank approximation of the input matrix. 
 * The assumption here is that the primary activity of the matrix can be quantified by the largest component of variance amongst its features.
 * (If this was not the case, one could argue that this feature set is not well-suited to capture the biology attributed to it.)
 * In effect, the rotation vector defines weights for all features in the set, focusing on genes that contribute to the primary activity.
 *
 * @tparam Value_ Floating-point type for the data.
 * @tparam Index_ Integer type for the indices.
 * @tparam Float_ Floating-point type for the output.
 *
 * @param[in] matrix An input **tatami** matrix.
 * Columns should contain cells while rows should contain genes.
 * @param options Further options. 
 * @param[out] output Collection of buffers in which to store the scores and weights.
 */
template<typename Value_, typename Index_, typename Float_>
void compute(const tatami::Matrix<Value_, Index_>& matrix, const Options& options, const Buffers<Float_>& output) {
    if (internal::check_edge_cases(matrix, options.rank, output)) {
        return;
    }

    scran_pca::SimplePcaOptions sopt;
    sopt.number = options.rank;
    sopt.scale = options.scale;
    sopt.realize_matrix = options.realize_matrix;
    sopt.num_threads = options.num_threads;
    sopt.irlba_options = options.irlba_options;
    auto res = scran_pca::simple_pca(matrix, sopt);

    double shift = std::accumulate(res.center.begin(), res.center.end(), 0.0) / matrix.nrow();
    std::fill_n(output.scores, matrix.ncol(), shift);
    internal::process_output(res.rotation, res.components, options.scale, res.scale, output);
}

/**
 * Overload of `compute()` that allocates memory for the results.
 *
 * @tparam Float_ Floating-point type for the output.
 * @tparam Value_ Floating-point type for the data.
 * @tparam Index_ Integer type for the indices.
 *
 * @param[in] matrix An input **tatami** matrix.
 * Columns should contain cells while rows should contain genes.
 * @param options Further options. 
 *
 * @return Results of the gene set score calculation.
 */
template<typename Float_ = double, typename Value_, typename Index_>
Results<Float_> compute(const tatami::Matrix<Value_, Index_>& matrix, const Options& options) {
    Results<Float_> output;
    output.weights.resize(matrix.nrow());
    output.scores.resize(matrix.ncol());

    Buffers<Float_> buffers;
    buffers.weights = output.weights.data();
    buffers.scores = output.scores.data();

    compute(matrix, options, buffers);
    return output;
}

}

#endif
