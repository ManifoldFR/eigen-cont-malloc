cmake_minimum_required(VERSION 3.16)


project(eigen_cont_malloc)



find_package(Eigen3 REQUIRED)


add_executable(main main.cpp)
target_link_libraries(main PRIVATE Eigen3::Eigen)

