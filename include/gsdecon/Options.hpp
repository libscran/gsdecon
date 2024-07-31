#ifndef GSDECON_OPTIONS_HPP
#define GSDECON_OPTIONS_HPP

#include "scran_blocks/scran_blocks.hpp"
#include "irlba/irlba.hpp"

/**
 * @file Options.hpp
 * @brief Options for the **gsdecon** algorithm.
 */

namespace gsdecon {

/**
 * @brief Options for `compute()`.
 */
struct Options {
    /**
     * @cond
     */
    Options() {
        irlba_options.cap_number = true;
    }
    /**
     * @endcond
     */

    /**
     * Rank of the low-rank approximation.
     */
    int rank = 1;

    /**
     * Should genes be scaled to unit variance?
     * Genes with zero variance are ignored.
     */
    bool scale = false;

    /**
     * Policy to use for weighting batches of different size, for `compute_blocked()`.
     */
    scran_blocks::WeightPolicy block_weight_policy = scran_blocks::WeightPolicy::VARIABLE;

    /**
     * Parameters for the variable block weights for `compute_blocked().
     * Only used when `BlockedPcaOptions::block_weight_policy = scran_blocks::WeightPolicy::VARIABLE`.
     */
    scran_blocks::VariableWeightParameters variable_block_weight_parameters;

    /**
     * Number of threads to use.
     */
    int num_threads = 1;

    /**
     * Whether to realize `tatami::Matrix` objects into an appropriate in-memory format before PCA.
     * This is typically faster but increases memory usage.
     */
    bool realize_matrix = true;

    /**
     * Further options to pass to `irlba::compute()`.
     */
    irlba::Options irlba_options;
};

}

#endif
