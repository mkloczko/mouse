//
// Created by mkloczko on 20.03.17.
//

#include "Histogram2DTest.hpp"

void histogram2DTestSuite(){
    normalization2DTest<10,10>();
    normalization2DTest<10,1000>();

    rangeNormalization2DTest<10,10>();
    rangeNormalization2DTest<10,1000>();
}