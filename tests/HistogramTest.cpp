//
// Created by mkloczko on 14.03.17.
//

#include "HistogramTest.hpp"

void histogramTestSuite(){
    medianTest<10>();
    medianTest<100>();
    medianTest<1000>();

    normalizationTest<10>();
    normalizationTest<100>();
    normalizationTest<1000>();
}