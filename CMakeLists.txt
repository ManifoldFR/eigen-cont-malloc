cmake_minimum_required(VERSION 3.16)


project(eigen_cont_malloc LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)


find_package(Eigen3 REQUIRED)
find_package(benchmark REQUIRED)


file(GLOB UTIL_SOURCES CONFIGURE_DEPENDS src/lib/*.cpp)
message(STATUS "Util sources: ${UTIL_SOURCES}")
add_library(utils SHARED ${UTIL_SOURCES})
target_link_libraries(utils PUBLIC Eigen3::Eigen fmt::fmt)
target_link_libraries(utils PUBLIC OpenMP::OpenMP_CXX TBB::tbb TBB::tbbmalloc)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE utils)

find_package(OpenMP REQUIRED)
find_package(TBB REQUIRED)
find_package(fmt REQUIRED)
add_compile_definitions(EIGEN_DONT_PARALLELIZE)

function(add_par_target file name)
  add_executable(${name} ${file})
  target_link_libraries(${name} PRIVATE utils benchmark::benchmark)
endfunction()

add_par_target(src/parallel.cpp parallel)
