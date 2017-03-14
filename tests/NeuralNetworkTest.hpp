//
// Created by mkloczko on 11.03.17.
//

#ifndef CHALLENGE_NEURALNETWORKTEST_HPP
#define CHALLENGE_NEURALNETWORKTEST_HPP


#include "../src/NeuralNetwork.hpp"


#include <rapidcheck.h>
#include <vector>
#include <iostream>
#include <string>
#include <memory>

using std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::snprintf;
using std::unique_ptr;

template <int I, int M, int O>
void gradientCheck(){
    string format("Checking the backpropagation<%d,%d,%d> algorithm...");
    size_t size = snprintf( nullptr, 0, format.c_str(), I,M,O ) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), I,M,O );

    double error = 0;
    int ix = 0;
    rc::check(buf.get()
            ,[&error, &ix]() {
                if(ix % 10 == 0){
                    cerr << ix << "...";
                }
                if(ix == 99){
                    cerr << endl;
                }
                ix +=1;

                unsigned long no_samples = *rc::gen::inRange(100,1000);

                //Since rapid check does not support doubles, we do this:
                vector<int> xs_int = *rc::gen::container<vector<int>>(no_samples * I, rc::gen::inRange<int>(0,1000));
                vector<int> ys_int = *rc::gen::container<vector<int>>(no_samples * O, rc::gen::inRange<int>(0,1));

                int    range_int = *rc::gen::inRange(1,100);
                double range     = ((double)range_int)/10.0;

                //Convert into the sets
                NeuralNetwork<I,M,O> start;
                start.weights_0 = range * Weights<M,I+1>::Random();
                start.weights_1 = range * Weights<O,M+1>::Random();
                TrainingSet<I> x_set(I,no_samples);
                TrainingSet<O> y_set(O,no_samples);

                for(unsigned long i = 0; i < no_samples; i++){
                    for (unsigned long in = 0; in < I; in++){
                        x_set(0,in) = ((double)xs_int[i + in])/1000.0;
                    }

                    for (unsigned long out = 0; out < O; out++){
                        y_set(0,out) = ((double)ys_int[i + out])/1000.0;
                    }

                }




                NeuralNetwork<I,M,O> backprop = backpropagation( start
                        , x_set
                        , y_set);

                NeuralNetwork<I,M,O> numerical = numerical_derivative( start
                        , x_set
                        , y_set);
                NeuralNetwork<I,M,O> diff;
                diff.weights_0 = numerical.weights_0 - backprop.weights_0;
                diff.weights_1 = numerical.weights_1 - backprop.weights_1;

                bool is_ok = true;
                double max_diff = std::numeric_limits<double>::min();
                for (unsigned long i = 0; i < diff.weights_0.size() && is_ok; i++){
                    max_diff = std::max(abs(diff.weights_0(i)),max_diff);
                    is_ok = abs(diff.weights_0(i)) < 0.001;
                }

                for (unsigned long i = 0; i < diff.weights_1.size() && is_ok; i++){
                    max_diff = std::max(abs(diff.weights_1(i)),max_diff);
                    is_ok = abs(diff.weights_1(i)) < 0.001;
                }
                error = std::max(error, max_diff);
                if (!is_ok){
                    cerr << "Max difference: " << max_diff << endl;
//                    cerr << "Start:\n " << start.weights_0 << "\n\n" << start.weights_1 << endl;
//                    cerr << "Weights 0:\n " << numerical.weights_0 << "\n\n" << backprop.weights_0 << endl;
//                    cerr << "Weights 1:\n " << numerical.weights_1 << "\n\n" << backprop.weights_1 << endl;
                }
                return is_ok;
            });
    cerr << "The maximum difference between the backprop and numeric derivative was: " << error << endl;
}

template <int I, int M, int O>
void gradientCheckRegularised(){
    string format("Checking the backpropagation<%d,%d,%d> algorithm...");
    size_t size = snprintf( nullptr, 0, format.c_str(), I,M,O ) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), I,M,O );

    double error = 0;
    int ix = 0;
    rc::check(buf.get()
            ,[&error, &ix]() {
                if(ix % 10 == 0){
                    cerr << ix << "...";
                }
                if(ix == 99){
                    cerr << endl;
                }
                ix +=1;

                unsigned long no_samples = *rc::gen::inRange(100,1000);

                //Since rapid check does not support doubles, we do this:
                vector<int> xs_int = *rc::gen::container<vector<int>>(no_samples * I, rc::gen::inRange<int>(0,1000));
                vector<int> ys_int = *rc::gen::container<vector<int>>(no_samples * O, rc::gen::inRange<int>(0,1));

                int    range_int = *rc::gen::inRange(1,100);
                double range     = ((double)range_int)/10.0;

                //Convert into the sets
                NeuralNetwork<I,M,O> start;
                start.weights_0 = range * Weights<M,I+1>::Random();
                start.weights_1 = range * Weights<O,M+1>::Random();
                TrainingSet<I> x_set(I,no_samples);
                TrainingSet<O> y_set(O,no_samples);

                for(unsigned long i = 0; i < no_samples; i++){
                    for (unsigned long in = 0; in < I; in++){
                        x_set(0,in) = ((double)xs_int[i + in])/1000.0;
                    }

                    for (unsigned long out = 0; out < O; out++) {
                        y_set(0, out) = ((double) ys_int[i + out]) / 1000.0;
                    }
                }



                int lambda_int = *rc::gen::inRange(1,10000);
                double lambda  = ((double)lambda_int)/100;

                NeuralNetwork<I,M,O> backprop =
                        backpropagation( start
                                , x_set
                                , y_set
                                , lambda);

                NeuralNetwork<I,M,O> numerical =
                        numerical_derivative( start
                                , x_set
                                , y_set
                                , lambda
                                , 0.00005);
                NeuralNetwork<I,M,O> diff;
                diff.weights_0 = numerical.weights_0 - backprop.weights_0;
                diff.weights_1 = numerical.weights_1 - backprop.weights_1;

                bool is_ok = true;
                double max_diff = std::numeric_limits<double>::min();
                for (unsigned long i = 0; i < diff.weights_0.size() && is_ok; i++){
                    max_diff = std::max(abs(diff.weights_0(i)),max_diff);
                    is_ok = abs(diff.weights_0(i)) < 0.001;
                }

                for (unsigned long i = 0; i < diff.weights_1.size() && is_ok; i++){
                    max_diff = std::max(abs(diff.weights_1(i)),max_diff);
                    is_ok = abs(diff.weights_1(i)) < 0.001;
                }

                error = std::max(error, max_diff);

                if (!is_ok){
                    cerr << "Max difference: " << max_diff << endl;
//                    cerr << "Start:\n " << start.weights_0 << "\n\n" << start.weights_1 << endl;
//                    cerr << "Weights 0:\n " << numerical.weights_0 << "\n\n" << backprop.weights_0 << endl;
//                    cerr << "Weights 1:\n " << numerical.weights_1 << "\n\n" << backprop.weights_1 << endl;
                }
                return is_ok;
            });
    cerr << "The maximum difference between the backprop and numeric derivative was: " << error << endl;
}

template <int I, int M, int O>
void gradientDescent(unsigned long max_iters){
    string format("Checking whether gradient_descent<%d,%d,%d> goes towards a minima...");
    size_t size = snprintf( nullptr, 0, format.c_str(), I,M,O ) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), I,M,O );

    double max_difference = 0;
    int ix = 0;
    rc::check(buf.get()
            , [&ix, &max_difference, max_iters](){
                if(ix % 10 == 0){
                    cerr << ix << "...";
                }
                if(ix == 99){
                    cerr << endl;
                }
                ix +=1;
                unsigned long no_samples = *rc::gen::inRange(50,500);

                //Since rapid check does not support doubles, we do this:
                vector<int> xs_int = *rc::gen::container<vector<int>>(no_samples * I, rc::gen::inRange<int>(0,1000));
                vector<int> ys_int = *rc::gen::container<vector<int>>(no_samples * O, rc::gen::inRange<int>(0,1));

                int    range_int = *rc::gen::inRange(1,100);
                double range     = ((double)range_int)/10.0;

                //Convert into the sets
                NeuralNetwork<I,M,O> start;
                start.weights_0 = range * Weights<M,I+1>::Random();
                start.weights_1 = range * Weights<O,M+1>::Random();
                TrainingSet<I> x_set(I,no_samples);
                TrainingSet<O> y_set(O,no_samples);

                for(unsigned long i = 0; i < no_samples; i++){
                    for (unsigned long in = 0; in < I; in++){
                        x_set(0,in) = ((double)xs_int[i + in])/1000.0;
                    }

                    for (unsigned long out = 0; out < O; out++){
                        y_set(0,out) = ((double)ys_int[i + out])/1000.0;
                    }

                }

                int lambda_int = *rc::gen::inRange(0,10000);
                double lambda  = ((double)lambda_int)/10;

                //There can be a case where alpha is too big and it can't converge.
                //Hopefully we won't hit it with a really small alpha.
                double alphas[5] = {0.01, 0.0025, 0.00025, 0.0000001};

                double at_first = costFunction(start       , x_set, y_set, lambda);
                for (double alpha : alphas){
                    NeuralNetwork<I,M,O> local_minima = gradient_descent(start, x_set, y_set, lambda, alpha, 0.001, max_iters);

                    double and_now  = costFunction(local_minima, x_set, y_set, lambda);

                    if(and_now <= at_first) {
                        max_difference = std::max(max_difference, at_first-and_now);
                        return true;
                    }
                }

                cerr << endl;
                return false;
            });
    cerr << "The maximum difference found for the cost functions was: " << max_difference << endl;
}

void neuralNetworkTestSuite();

#endif //CHALLENGE_NEURALNETWORKTEST_HPP
