cmake_minimum_required(VERSION 3.5)
project(challenge)

#add_subdirectory(eigen)
include_directories(${CMAKE_SOURCE_DIR}/eigen)
include_directories(${CMAKE_SOURCE_DIR}/fast-cpp-csv-parser)
add_subdirectory(rapidcheck)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -DEIGEN_MPL2_ONLY -DCSV_IO_NO_THREAD -std=c++11")

set(NNETWORK_FILES src/NeuralNetwork.hpp src/WordDictionary.hpp src/Histogram.hpp)
set(NNETWORK_TEST  tests/test.cpp tests/NeuralNetworkTest.hpp tests/HistogramTest.hpp tests/HistogramTest.cpp tests/NeuralNetworkTest.cpp tests/Histogram2DTest.hpp tests/Histogram2DTest.cpp)

add_executable(tests     ${NNETWORK_FILES} ${NNETWORK_TEST})
target_link_libraries(tests rapidcheck)

add_executable(train             ${NNETWORK_FILES} train.cpp            )
add_executable(train_complex     ${NNETWORK_FILES} train_complex.cpp    )
add_executable(challenge         ${NNETWORK_FILES} challenge.cpp        )
add_executable(challenge_complex ${NNETWORK_FILES} challenge_complex.cpp)

