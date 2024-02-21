cmake_minimum_required(VERSION 3.16)


project(eigen_cont_malloc LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)


find_package(Eigen3 REQUIRED)
find_package(benchmark REQUIRED)
find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)
find_package(TBB REQUIRED)
add_compile_definitions(EIGEN_DONT_PARALLELIZE)


file(GLOB UTIL_SOURCES CONFIGURE_DEPENDS src/lib/*.cpp)
message(STATUS "Util sources: ${UTIL_SOURCES}")
add_library(utils SHARED ${UTIL_SOURCES})

target_link_libraries(utils PUBLIC Eigen3::Eigen fmt::fmt)
target_link_libraries(utils PUBLIC OpenMP::OpenMP_CXX TBB::tbb TBB::tbbmalloc)



function(my_add_target file)
  get_filename_component(name ${file} NAME_WE)
  add_executable(${name} ${file})
  target_link_libraries(${name} PRIVATE utils)
endfunction()

my_add_target(src/main.cpp)
my_add_target(src/bench.cpp)
my_add_target(src/test_pmr.cpp)
target_link_libraries(bench PRIVATE benchmark::benchmark)
