//
// Created by mkloczko on 12.03.17.
//

#include "NeuralNetworkTest.hpp"

#include <rapidcheck.h>
#include <vector>
#include <iostream>

using std::vector;
using std::cerr;
using std::endl;

void gradientCheck(){
    double error = 0;
//    int ix = 0;
    rc::check("Checking the backpropagation algorithm..."
            ,[&error]() {
                unsigned long no_samples = *rc::gen::inRange(100,1000);

                //Since rapid check does not support doubles, we do this:
                vector<int> xs_int = *rc::gen::container<vector<int>>(no_samples * 4, rc::gen::inRange<int>(0,1000));
                vector<int> ys_int = *rc::gen::container<vector<int>>(no_samples * 2, rc::gen::inRange<int>(0,1));

                int    range_int = *rc::gen::inRange(1,100);
                double range     = ((double)range_int)/10.0;

                //Convert into the sets
                NeuralNetwork start;
                start.weights_0 = range * Weights<5,5>::Random();
                start.weights_1 = range * Weights<2,6>::Random();
                TrainingSet<4> x_set(4,no_samples);
                TrainingSet<2> y_set(2,no_samples);

                for(unsigned long i = 0; i < no_samples; i++){
                    x_set(0,i) = ((double)xs_int[i + 0])/1000.0;
                    x_set(1,i) = ((double)xs_int[i + 1])/1000.0;
                    x_set(2,i) = ((double)xs_int[i + 2])/1000.0;
                    x_set(3,i) = ((double)xs_int[i + 3])/1000.0;

                    y_set(0,i) = ((double)ys_int[i + 0]);
                    y_set(1,i) = ((double)ys_int[i + 1]);
                }




                NeuralNetwork backprop = backpropagation( start
                        , x_set
                        , y_set);

                NeuralNetwork numerical = numerical_derivative( start
                        , x_set
                        , y_set);
                NeuralNetwork diff;
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
                    cerr << "Start:\n " << start.weights_0 << "\n\n" << start.weights_1 << endl;
                    cerr << "Weights 0:\n " << numerical.weights_0 << "\n\n" << backprop.weights_0 << endl;
                    cerr << "Weights 1:\n " << numerical.weights_1 << "\n\n" << backprop.weights_1 << endl;
                }
                return is_ok;
            });
    cerr << "The maximum difference between the algorithms was: " << error << endl;
}


void gradientCheckRegularised(){
    double error = 0;
    rc::check("Checking the backpropagation algorithm with regularisation ..."
            ,[&error]() {
                unsigned long no_samples = *rc::gen::inRange(100,1000);

                //Since rapid check does not support doubles, we do this:
                vector<int> xs_int = *rc::gen::container<vector<int>>(no_samples * 4, rc::gen::inRange<int>(0,1000));
                vector<int> ys_int = *rc::gen::container<vector<int>>(no_samples * 2, rc::gen::inRange<int>(0,1));

                int    range_int = *rc::gen::inRange(1,100);
                double range     = ((double)range_int)/10.0;

                //Convert into the sets
                NeuralNetwork start;
                start.weights_0 = range * Weights<5,5>::Random();
                start.weights_1 = range * Weights<2,6>::Random();
                TrainingSet<4> x_set(4,no_samples);
                TrainingSet<2> y_set(2,no_samples);

                for(unsigned long i = 0; i < no_samples; i++){
                    x_set(0,i) = ((double)xs_int[i + 0])/1000.0;
                    x_set(1,i) = ((double)xs_int[i + 1])/1000.0;
                    x_set(2,i) = ((double)xs_int[i + 2])/1000.0;
                    x_set(3,i) = ((double)xs_int[i + 3])/1000.0;

                    y_set(0,i) = ((double)ys_int[i + 0]);
                    y_set(1,i) = ((double)ys_int[i + 1]);
                }


                int lambda_int = *rc::gen::inRange(0,10000);
                double lambda  = ((double)lambda_int)/10;

                NeuralNetwork backprop =
                        backpropagation( start
                                , x_set
                                , y_set
                                , lambda);

                NeuralNetwork numerical =
                        numerical_derivative( start
                                , x_set
                                , y_set
                                , lambda
                                , 0.00005);
                NeuralNetwork diff;
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
                    cerr << "Start:\n " << start.weights_0 << "\n\n" << start.weights_1 << endl;
                    cerr << "Weights 0:\n " << numerical.weights_0 << "\n\n" << backprop.weights_0 << endl;
                    cerr << "Weights 1:\n " << numerical.weights_1 << "\n\n" << backprop.weights_1 << endl;
                }
                return is_ok;
            });
    cerr << "The maximum difference between the algorithms was: " << error << endl;
}