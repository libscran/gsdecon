# C++ port of the GSDecon algorithm

![Unit tests](https://github.com/libscran/gsdecon/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/libscran/gsdecon/actions/workflows/doxygenate.yaml/badge.svg)
[![Codecov](https://codecov.io/gh/libscran/gsdecon/graph/badge.svg?token=qklLZtJSE9)](https://codecov.io/gh/libscran/gsdecon)

## Introduction

This repository implements a C++ port of the [**GSDecon**](https://github.com/JasonHackney/GSDecon) algorithm for computing gene set scores from a single-cell expression matrix.
The assumption is that there is one main axis of variation for the genes in the set, typically corresponding to some coordinated biological pathway.
To capture this variation, we compute the first principal component (PC) in the submatrix consisting of the genes of interest.
We then construct a rank-1 approximation of the submatrix and define the gene set score for each cell from the column means of the approximated matrix.
In effect, each cell's score is the average expression of each gene in the set after removing noise and other secondary factors of variation from the expression submatrix -
this corresponds to the expression of an "eigengene" in the context of the original **GSDecon**. 

## Quick start

Given a [`tatami::Matrix`](https://github.com/tatami-inc/tatami) corresponding to the submatrix of genes in the set, 
the `gsdecon::compute()` function will compute the scores for each cell:

```cpp
#include "gsdecon/gsdecon.hpp"

// This should already be subsetted to the features of interest:
const tatami::Matrix<double, int>& mat = some_data_source();

gsdecon::Options opt;
auto res = gsdecon::compute(mat, opt);
res.scores; // one per cell.
res.weights; // one per gene in the set.
```

The per-gene weights are set to the absolute values of the rotation matrix corresponding to the first PC.
They can be used to obtain some diagnostics about the genes with the greatest contribution to the rank-1 approximation.

In the case of multiple blocks (e.g., samples, batches), we regress out the block effects and compute the first PC from the residuals.
This ensures that the major axis of variation is not driven by uninteresting differences between blocks.
However, this does not actually remove the block effects themselves, so any further analysis steps should still be block-aware.

```cpp
gsdecon::Options opt;
std::vector<double> block_ids(mat.ncols()); // fill with block assignments.
auto bres = gsdecon::compute_blocked(mat, block_ids.data(), opt);
```

Check out the [reference documentation](https://libscran.github.io/gsdecon) for more details.

## Building projects

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  gsdecon
  GIT_REPOSITORY https://github.com/libscran/gsdecon
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(gsdecon)
```

Then you can link to **gsdecon** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe libscran::gsdecon)

# For libaries
target_link_libraries(mylib INTERFACE libscran::gsdecon)
```

### CMake with `find_package()`

```cmake
find_package(libscran_gsdecon CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE libscran::gsdecon)
```

To install the library, use:

```sh
mkdir build && cd build
cmake .. -DGSDECON_TESTS=OFF
cmake --build . --target install
```

By default, this will use `FetchContent` to fetch all external dependencies.
If you want to install them manually, use `-DGSDECON_FETCH_EXTERN=OFF`.
See the tags in [`extern/CMakeLists.txt`](extern/CMakeLists.txt) to find compatible versions of each dependency.

### Manual

If you're not using CMake, the simple approach is to just copy the files in `include/` - either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
This requires the external dependencies listed in [`extern/CMakeLists.txt`](extern/CMakeLists.txt), which also need to be made available during compilation.
