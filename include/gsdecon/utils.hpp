#ifndef GSDECON_UTILS_HPP
#define GSDECON_UTILS_HPP

#include <algorithm>
#include "Eigen/Dense"

#include "Results.hpp"

namespace gsdecon {

namespace internal {

template<typename Value_, typename Index_, typename Float_>
bool check_edge_cases(const tatami::Matrix<Value_, Index_>& matrix, int rank, const Buffers<Float_>& output) {
    auto NR = matrix.nrow();
    auto NC = matrix.ncol();
    if (NR == 0) {
        std::fill_n(output.scores, NC, 0.0);
        return true;
    }

    if (NR == 1) {
        output.weights[0] = 1;
        auto ext = matrix.dense_row();
        if constexpr(std::is_same<Value_, Float_>::value) {
            auto ptr = ext->fetch(0, output.scores);
            tatami::copy_n(ptr, NC, output.scores);
        } else {
            std::vector<Value_> buffer(NC);
            auto ptr = ext->fetch(0, buffer.data());
            std::copy_n(ptr, NC, output.scores);
        }
        return true;
    }

    if (NC == 0) {
        std::fill_n(output.weights, NR, 0.0); 
        return true;
    }

    if (rank == 0) {
        std::fill_n(output.scores, NC, 0.0); 
        std::fill_n(output.weights, NR, 0.0); 
        return true;
    }

    return false;
}

template<typename Float_>
void process_output(const Eigen::MatrixXd& rotation, const Eigen::MatrixXd& components, bool scale, const Eigen::VectorXd& scale_v, const Buffers<Float_>& output) {
    size_t npcs = rotation.cols();
    size_t nfeat = rotation.rows();
    size_t ncells = components.cols();
    static_assert(!Eigen::MatrixXd::IsRowMajor); // just double-checking...

    if (npcs > 1) {
        std::vector<double> multipliers(npcs);
        std::fill_n(output.weights, nfeat, 0);
        for (size_t pc = 0; pc < npcs; ++pc) {
            const double* rptr = rotation.data() + pc * nfeat; 

#ifdef _OPENMP
            #pragma omp simd
#endif
            for (size_t i = 0; i < nfeat; ++i) {
                auto val = rptr[i];
                output.weights[i] += val * val;
            }

            /*
             * We have the first PC 'P' and a column of the rotation vector 'R',
             * plus a centering vector 'C' and scaling vector 'S'. The low-rank
             * approximation is defined as (using R syntax):
             *
             *     L = outer(R, P) * S + C 
             *       = outer(R * S, P) + C
             *
             * Remember that we want the column means of the rank-1 approximation, so:
             *
             *     colMeans(L) = mean(R * S) * P + colMeans(C)
             *
             * If scale = false, then S can be dropped from the above expression.
             */
            if (scale) {
                multipliers[pc] = std::inner_product(rptr, rptr + nfeat, scale_v.data(), 0.0);
            } else {
                multipliers[pc] = std::accumulate(rptr, rptr + nfeat, 0.0);
            }
            multipliers[pc] /= nfeat;
        }

        Float_ denom = npcs;
#ifdef _OPENMP
        #pragma omp simd
#endif
        for (size_t i = 0; i < nfeat; ++i) {
            output.weights[i] = std::sqrt(output.weights[i] / denom);
        }

#ifdef _OPENMP
        #pragma omp parallel for
#endif
        for (size_t c = 0; c < ncells; ++c) {
            const double* cptr = components.data() + c * npcs;
            output.scores[c] += std::inner_product(multipliers.begin(), multipliers.end(), cptr, 0.0);
        }

    } else {
        const double* rptr = rotation.data();
        for (size_t i = 0; i < nfeat; ++i) {
            output.weights[i] = std::abs(rptr[i]);
        }

        double multiplier;
        if (scale) {
            multiplier = std::inner_product(rptr, rptr + nfeat, scale_v.data(), 0.0);
        } else {
            multiplier = std::accumulate(rptr, rptr + nfeat, 0.0);
        }
        multiplier /= nfeat;

        const double* cptr = components.data();
#ifdef _OPENMP
        #pragma omp simd
#endif
        for (size_t c = 0; c < ncells; ++c) {
            output.scores[c] += cptr[c] * multiplier;
        }
    }
}

}

}

#endif
