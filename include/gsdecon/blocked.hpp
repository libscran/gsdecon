#ifndef GSDECON_BLOCKED_HPP
#define GSDECON_BLOCKED_HPP

#include <vector>

#include "Eigen/Dense"

#include "tatami/tatami.hpp"
#include "irlba/irlba.hpp"
#include "scran_pca/scran_pca.hpp"

#include "Options.hpp"
#include "Results.hpp"
#include "utils.hpp"

/**
 * @file blocked.hpp
 * @brief Compute per-cell scores with blocking.
 */

namespace gsdecon {

/**
 * Extension of the algorithm described in `compute()` to datasets containing multiple blocks (e.g., batches, samples).
 *
 * In the presence of strong block effects, naively running `compute()` would yield a first PC that is driven by uninteresting inter-block differences.
 * Here, We werform the PCA on the residuals after centering each block, ensuring that the first PC focuses on the interesting variation within each block.
 * Blocks can also be weighted so that they contribute equally to the rotation vector, regardless of the number of cells.
 *
 * Note that the purpose of the blocking is to ensure that inter-block differences do not drive the first few PCs, not to remove the block effects themselves.
 * Using residuals for batch correction requires strong assumptions such as identical block composition and consistent shifts across subpopulations, and we don't attempt make that claim.
 * The caller is instead responsible for ensuring that the block structure is still considered in any further analysis of the returned scores.
 *
 * @tparam Value_ Floating-point type for the data.
 * @tparam Index_ Integer type for the indices.
 * @tparam Block_ Integer type for the block assignments.
 * @tparam Float_ Floating-point type for the output.
 *
 * @param[in] matrix An input **tatami** matrix.
 * Columns should contain cells while rows should contain genes.
 * @param[in] block Pointer to an array of length equal to the number of columns in `matrix`.
 * This should contain the blocking factor as 0-based block assignments 
 * (i.e., for \f$N\f$ blocks, block identities should run from 0 to \f$N-1\f$ with at least one entry for each block.)
 * @param options Further options. 
 * @param[out] output Collection of buffers in which to store the scores and weights.
 */
template<typename Value_, typename Index_, typename Block_, typename Float_>
void compute_blocked(const tatami::Matrix<Value_, Index_>& matrix, const Block_* block, const Options& options, const Buffers<Float_>& output) {
    if (internal::check_edge_cases(matrix, options.rank, output)) {
        return;
    }

    scran_pca::BlockedPcaOptions bopt;
    bopt.number = options.rank;
    bopt.scale = options.scale;
    bopt.block_weight_policy = options.block_weight_policy;
    bopt.variable_block_weight_parameters = options.variable_block_weight_parameters;
    bopt.realize_matrix = options.realize_matrix;
    bopt.num_threads = options.num_threads;
    bopt.irlba_options = options.irlba_options;
    auto res = scran_pca::blocked_pca(matrix, block, bopt);

    // Here, we restore the block-specific centers. Don't be tempted into
    // using MultiBatchPca, as that doesn't yield a rank-1 approximation
    // that preserves global shifts between blocks.
    static_assert(!Eigen::MatrixXd::IsRowMajor); // just double-checking...
    const double* cptr = res.center.data();
    size_t nfeat = res.center.cols();
    size_t nblocks = res.center.rows();
    std::vector<double> block_means(nblocks);

    for (size_t f = 0; f < nfeat; ++f, cptr += nblocks) {
#ifdef _OPENMP
        #pragma omp simd
#endif
        for (size_t b = 0; b < nblocks; ++b) {
            block_means[b] += cptr[b];
        }
    }
    double denom = nfeat;
    for (auto& b : block_means) {
        b /= denom;
    }

    size_t ncells = res.components.cols();
#ifdef _OPENMP
    #pragma omp simd
#endif
    for (size_t c = 0; c < ncells; ++c) {
        output.scores[c] = block_means[block[c]];
    }
    internal::process_output(res.rotation, res.components, options.scale, res.scale, output);
}

/**
 * Overload of `compute_blocked()` that allocates memory for the results.
 *
 * @tparam Float_ Floating-point type for the output.
 * @tparam Value_ Floating-point type for the data.
 * @tparam Index_ Integer type for the indices.
 * @tparam Block_ Integer type for the block assignments.
 *
 * @param[in] matrix An input **tatami** matrix.
 * Columns should contain cells while rows should contain genes.
 * @param[in] block Pointer to an array of length equal to the number of columns in `matrix`.
 * This should contain the blocking factor as 0-based block assignments 
 * (i.e., for \f$N\f$ blocks, block identities should run from 0 to \f$N-1\f$ with at least one entry for each block.)
 * @param options Further options. 
 *
 * @return Results of the gene set score calculation.
 */
template<typename Float_ = double, typename Value_, typename Index_, typename Block_>
Results<Float_> compute_blocked(const tatami::Matrix<Value_, Index_>& matrix, const Block_* block, const Options& options) {
    Results<Float_> output;
    output.weights.resize(matrix.nrow());
    output.scores.resize(matrix.ncol());

    Buffers<Float_> buffers;
    buffers.weights = output.weights.data();
    buffers.scores = output.scores.data();

    compute_blocked(matrix, block, options, buffers);
    return output;
}

}

#endif
