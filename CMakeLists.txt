cmake_minimum_required(VERSION 3.14)

project(gsdecon
    VERSION 0.1.0
    DESCRIPTION "Gene set scoring with principal components"
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Library
add_library(gsdecon INTERFACE)
add_library(libscran::gsdecon ALIAS gsdecon)

target_include_directories(gsdecon INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/gsdecon>)
target_compile_features(gsdecon INTERFACE cxx_std_17)

# Dependencies
option(GSDECON_FETCH_EXTERN "Automatically fetch gsdecon's external dependencies." ON)
if(GSDECON_FETCH_EXTERN)
    add_subdirectory(extern)
else()
    find_package(tatami_tatami 3.0.0 CONFIG REQUIRED)
    find_package(libscran_scran_pca 0.1.0 CONFIG REQUIRED)
    find_package(libscran_scran_blocks 0.1.0 CONFIG REQUIRED)
    find_package(ltla_irlba 2.0.0 CONFIG REQUIRED)
    find_package(Eigen3 3.4.0 REQUIRED NO_MODULE)
endif()

target_link_libraries(
    gsdecon INTERFACE 
    tatami::tatami
    libscran::scran_pca
    libscran::scran_blocks
    ltla::irlba
    Eigen3::Eigen
)

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(GSDECON_TESTS "Build gsdecon's test suite." ON)
else()
    option(GSDECON_TESTS "Build gsdecon's test suite." OFF)
endif()

if(GSDECON_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

# Install
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gsdecon)

install(TARGETS gsdecon
    EXPORT gsdeconTargets)

install(EXPORT gsdeconTargets
    FILE libscran_gsdeconTargets.cmake
    NAMESPACE libscran::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscran_gsdecon)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/libscran_gsdeconConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscran_gsdecon)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libscran_gsdeconConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libscran_gsdeconConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libscran_gsdeconConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscran_gsdecon)
