//
// Created by mkloczko on 11.03.17.
//
#include "NeuralNetworkTest.hpp"

int main(){
//    gradientCheck<4,5,2>();
//    gradientCheckRegularised<4,5,2>();
//    gradientDescent<4,5,2>(100);

//    gradientCheck<10,12,8>();
//    gradientCheckRegularised<10,12,8>();
    gradientDescent<10,12,8>(40);

    gradientDescent<25,30,12>(40);
}