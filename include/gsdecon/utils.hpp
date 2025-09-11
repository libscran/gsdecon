#ifndef GSDECON_UTILS_HPP
#define GSDECON_UTILS_HPP

#include <cmath>
#include <algorithm>
#include <vector>
#include <type_traits>
#include <numeric>

#include "Eigen/Dense"
#include "sanisizer/sanisizer.hpp"

#include "Results.hpp"

namespace gsdecon {

template<typename Input_>
std::remove_cv_t<std::remove_reference_t<Input_> > I(const Input_ x) {
    return x;
}

namespace internal {

template<typename Value_, typename Index_, typename Float_>
bool check_edge_cases(const tatami::Matrix<Value_, Index_>& matrix, const int rank, const Buffers<Float_>& output) {
    const auto NR = matrix.nrow();
    const auto NC = matrix.ncol();
    if (NR == 0) {
        std::fill_n(output.scores, NC, 0.0);
        return true;
    }

    if (NR == 1) {
        output.weights[0] = 1;
        auto ext = matrix.dense_row();
        if constexpr(std::is_same<Value_, Float_>::value) {
            const auto ptr = ext->fetch(0, output.scores);
            tatami::copy_n(ptr, NC, output.scores);
        } else {
            auto buffer = sanisizer::create<std::vector<Value_> >(NC);
            const auto ptr = ext->fetch(0, buffer.data());
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
    const auto npcs = rotation.cols();
    const auto nfeat = rotation.rows();
    const auto ncells = components.cols();
    static_assert(!Eigen::MatrixXd::IsRowMajor); // just double-checking...

    if (npcs > 1) {
        auto multipliers = sanisizer::create<std::vector<Float_> >(npcs);
        std::fill_n(output.weights, nfeat, 0);
        for (decltype(I(npcs)) pc = 0; pc < npcs; ++pc) {
            const auto rptr = rotation.data() + sanisizer::product_unsafe<std::size_t>(pc, nfeat); 

#ifdef _OPENMP
            #pragma omp simd
#endif
            for (decltype(I(nfeat)) i = 0; i < nfeat; ++i) {
                const auto val = rptr[i];
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
                multipliers[pc] = std::inner_product(rptr, rptr + nfeat, scale_v.data(), static_cast<Float_>(0));
            } else {
                multipliers[pc] = std::accumulate(rptr, rptr + nfeat, static_cast<Float_>(0));
            }
            multipliers[pc] /= nfeat;
        }

#ifdef _OPENMP
        #pragma omp simd
#endif
        for (decltype(I(nfeat)) i = 0; i < nfeat; ++i) {
            output.weights[i] = std::sqrt(output.weights[i] / npcs);
        }

#ifdef _OPENMP
        #pragma omp parallel for
#endif
        for (decltype(I(ncells)) c = 0; c < ncells; ++c) {
            const auto cptr = components.data() + sanisizer::product_unsafe<std::size_t>(c, npcs);
            output.scores[c] += std::inner_product(multipliers.begin(), multipliers.end(), cptr, static_cast<Float_>(0));
        }

    } else {
        const auto rptr = rotation.data();
        for (decltype(I(nfeat)) i = 0; i < nfeat; ++i) {
            output.weights[i] = std::abs(rptr[i]);
        }

        Float_ multiplier;
        if (scale) {
            multiplier = std::inner_product(rptr, rptr + nfeat, scale_v.data(), static_cast<Float_>(0));
        } else {
            multiplier = std::accumulate(rptr, rptr + nfeat, static_cast<Float_>(0));
        }
        multiplier /= nfeat;

#ifdef _OPENMP
        #pragma omp simd
#endif
        for (decltype(I(ncells)) c = 0; c < ncells; ++c) {
            output.scores[c] += components.coeff(c) * multiplier;
        }
    }
}

}

}

#endif
