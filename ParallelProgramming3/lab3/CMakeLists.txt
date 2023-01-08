cmake_minimum_required(VERSION 3.16)
project(lab3)

set(CMAKE_CXX_STANDARD 20)
# added -fopenmp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")


#add_executable(lab3_parallel main.cpp lab_1/lab_1.cpp lab_2/lab_2.cpp lab_3/lab_3.cpp)
add_executable(lab3_parallel main.cpp)
