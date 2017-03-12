//
// Created by mkloczko on 11.03.17.
//

#ifndef CHALLENGE_NEURALNETWORKTEST_HPP
#define CHALLENGE_NEURALNETWORKTEST_HPP


#include "../src/NeuralNetwork.hpp"


void gradientCheck();
void gradientCheckRegularised();

void gradientDescent(unsigned long max_iters = 100);

#endif //CHALLENGE_NEURALNETWORKTEST_HPP
