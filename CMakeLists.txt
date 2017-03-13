cmake_minimum_required(VERSION 3.5)
project(challenge)

#add_subdirectory(eigen)
include_directories(${CMAKE_SOURCE_DIR}/eigen)
add_subdirectory(rapidcheck)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEIGEN_MPL2_ONLY -std=c++11")



set(NNETWORK_FILES src/NeuralNetwork.hpp tests/NeuralNetworkTest.hpp tests/test.cpp)
set(NNETWORK_TEST  tests/test.cpp tests/NeuralNetworkTest.hpp)

add_executable(challenge ${NNETWORK_FILES} main.cpp)
add_executable(tests     ${NNETWORK_FILES} ${NNETWORK_TEST})
target_link_libraries(tests rapidcheck)