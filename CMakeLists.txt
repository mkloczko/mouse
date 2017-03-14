cmake_minimum_required(VERSION 3.5)
project(challenge)

#add_subdirectory(eigen)
include_directories(${CMAKE_SOURCE_DIR}/eigen)
add_subdirectory(rapidcheck)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEIGEN_MPL2_ONLY -std=c++11")



set(NNETWORK_FILES src/NeuralNetwork.hpp src/WordDictionary.hpp src/Histogram.hpp)
set(NNETWORK_TEST  tests/test.cpp tests/NeuralNetworkTest.hpp tests/HistogramTest.hpp tests/HistogramTest.cpp tests/NeuralNetworkTest.cpp)

add_executable(challenge ${NNETWORK_FILES} main.cpp)
add_executable(tests     ${NNETWORK_FILES} ${NNETWORK_TEST})
target_link_libraries(tests rapidcheck)