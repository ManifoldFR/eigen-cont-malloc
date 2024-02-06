cmake_minimum_required(VERSION 3.16)


project(eigen_cont_malloc LANGUAGES CXX)


find_package(Eigen3 REQUIRED)
find_package(benchmark REQUIRED)


add_executable(main main.cpp)
target_link_libraries(main PRIVATE Eigen3::Eigen)
target_link_libraries(main PRIVATE benchmark::benchmark)

find_package(OpenMP REQUIRED)
find_package(TBB REQUIRED)

add_executable(parallel parallel.cpp)
target_link_libraries(parallel PRIVATE Eigen3::Eigen OpenMP::OpenMP_CXX TBB::tbb)
target_link_libraries(parallel PRIVATE benchmark::benchmark)
