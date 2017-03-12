//
// Created by mkloczko on 12.03.17.
//

#include "NeuralNetwork.hpp"
#include <iostream>

using std::cerr;
using std::endl;

Layer<2> NeuralNetwork::compute(Layer<4> input) const{
    return forwardProp(forwardProp(input, weights_0), weights_1);
}

TrainingSet<2> NeuralNetwork::compute(TrainingSet<4> input) const{
    return forwardProp(forwardProp(input, weights_0), weights_1);
}

//The function has to be numerically robust.
double costFunction( const NeuralNetwork & network
        , TrainingSet<4> training
        , TrainingSet<2> values
        , double lambda) {
    //Cost function
    auto hs     = network.compute(training);
    auto part1 = values.array() * hs.array().log() ;
    auto part2 = (1 - values.array()) * (1 - hs.array()).array().log();
    auto elems = - (part1 + part2);

    double sum = elems.sum()/training.cols();

    //Regularisation
    double reg = 0;
    reg += network.weights_0.rightCols<4>().unaryExpr([](double a){return a * a;}).sum();
    reg += network.weights_1.rightCols<5>().unaryExpr([](double a){return a * a;}).sum();
    reg = reg/(2*training.cols());
    reg = reg * lambda;

    return sum + reg;
}

NeuralNetwork backpropagation(const NeuralNetwork & network,
                              TrainingSet<4> training,
                              TrainingSet<2> values,
                              double lambda){
    if (training.cols() != values.cols()){
        throw std::invalid_argument("The number of X's is not equal to number of Y's.");
    }

    Weights<5, 5> delta_0 = Weights<5,5>::Zero();
    Weights<2, 6> delta_1 = Weights<2,6>::Zero();

    Layer<4> in_layer;
    Layer<5> mid_layer;
    Layer<2> out_layer;

    Layer<2> out_error;
    Layer<5> mid_error;

    for(unsigned long i = 0; i < training.cols(); i++){
        in_layer  = training.col(i);
        mid_layer = forwardProp(in_layer,  network.weights_0);
        out_layer = forwardProp(mid_layer, network.weights_1);

        out_error = out_layer - values.col(i);
        mid_error = (network.weights_1.transpose() * out_error ).template bottomRows<5>()
                            .array() * mid_layer.array();
        mid_error = mid_error.array() * (mid_layer.array() * (-1) + 1);


        delta_0 += mid_error*addBias(in_layer).transpose();
        delta_1 += out_error*addBias(mid_layer).transpose();
    }

    //Add the regularisation parameter
    delta_0.rightCols<4>() += lambda*network.weights_0.rightCols<4>();
    delta_1.rightCols<5>() += lambda*network.weights_1.rightCols<5>();

    //Divide by the number of training samples
    delta_0 /= training.cols();
    delta_1 /= training.cols();

    NeuralNetwork ret;
    ret.weights_0 = delta_0;
    ret.weights_1 = delta_1;

    return ret;
}

//Calculates the derivative of the cost function based on numerical derivatives
NeuralNetwork numerical_derivative( const NeuralNetwork & thetas
        , TrainingSet<4> training
        , TrainingSet<2> values
        , double lambda
        , double epsilon){

    NeuralNetwork ret;
    //Need to traverse through all Thetas.
    for(unsigned long i = 0; i < thetas.weights_0.size(); i++){
        NeuralNetwork thetas_plus  = thetas;
        NeuralNetwork thetas_minus = thetas;
        thetas_plus .weights_0(i) += epsilon;
        thetas_minus.weights_0(i) -= epsilon;

        double J_plus  = costFunction(thetas_plus , training, values, lambda);
        double J_minus = costFunction(thetas_minus, training, values, lambda);

        ret.weights_0(i) = (J_plus - J_minus)/(2*epsilon);
    }

    for(unsigned long i = 0; i < thetas.weights_1.size(); i++){
        NeuralNetwork thetas_plus  = thetas;
        NeuralNetwork thetas_minus = thetas;
        thetas_plus .weights_1(i) += epsilon;
        thetas_minus.weights_1(i) -= epsilon;

        double J_plus  = costFunction(thetas_plus , training, values, lambda);
        double J_minus = costFunction(thetas_minus, training, values, lambda);

        ret.weights_1(i) = (J_plus - J_minus)/(2*epsilon);
    }

    return ret;
}