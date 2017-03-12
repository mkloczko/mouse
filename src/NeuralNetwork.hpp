#include <iostream>
#include <exception>
#include <Eigen/Core>
#include <cmath>
//Activation function
//static double sigmoid(const double & v){
//        return 1 / ( 1 + std::exp(-v));
//}

//Type aliases for the neural network
template <int Rows>
using Layer = Eigen::Matrix<double, Rows, 1>;

template <int Rows, int Cols>
using Weights = Eigen::Matrix<double, Rows, Cols>;

template <int Rows>
using TrainingSet    = Eigen::Matrix<double, Rows, Eigen::Dynamic>;
using TrainingValues = Eigen::Matrix<double,    1, Eigen::Dynamic>;




//Forward propagation from the layer behind to the layer forward.
template <int Ret, int In>
Layer<Ret> forwardProp(const Layer<In> & in, const Weights<Ret, In+1> & weights){
    //Set the input layer with bias.
    Layer<In+1> input_biased = Layer<In+1>::Constant(1);
    input_biased.template bottomRows<In>() = in;

    auto rety = weights * input_biased;
    return rety.unaryExpr([](double a){
        double ret = 1.0 / ( 1.0 + std::exp(-a));
        return ret;
    });
};

template <int Ret, int In>
TrainingSet<Ret> forwardProp(const TrainingSet<In> & in, const Weights<Ret, In+1> & weights){
    //Set the input layer with bias.
    TrainingSet<In+1> input_biased(In+1, in.cols());
    input_biased.row(0).setOnes();
    input_biased.template bottomRows<In>() = in;

    auto rety = weights * input_biased;
    return rety.unaryExpr([](double a){return 1.0 / ( 1.0 + std::exp(-a));});
};


//Adding bias to layers.
template <int N>
constexpr Layer<N+1> addBias(const Layer<N> & in, const double & constant = 1){
    Layer<N+1> ret = Layer<N+1>::Constant(constant);
    ret.template bottomRows<N>() = in;
    return ret;
}

/*
 * The Neural Network itself.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Holds the weights.
 */
struct NeuralNetwork{

    Weights<5, 5> weights_0 = 0.001 * Weights<5, 5>::Random();
    Weights<2, 6> weights_1 = 0.001 * Weights<2, 6>::Random();

    Layer<2>       compute(Layer<4> input) const;
    TrainingSet<2> compute(TrainingSet<4> input) const;
};

//Calculates the derivative of the cost function based on the back propagation algorithm
NeuralNetwork backpropagation(const NeuralNetwork & network,
                              TrainingSet<4> training,
                              TrainingSet<2> values,
                              double lambda = 0);

//Calculates the cost function itself.
double        costFunction( const NeuralNetwork & network
                          , TrainingSet<4> training
                          , TrainingSet<2> values
                          , double lambda = 0);

//Calculates the derivative of the cost function based on numerical derivatives
NeuralNetwork numerical_derivative( const NeuralNetwork & thetas
                                  , TrainingSet<4> training
                                  , TrainingSet<2> values
                                  , double lambda = 0
                                  , double epsilon = 0.001);

NeuralNetwork gradient_descent( const NeuralNetwork & network
                              , TrainingSet<4> training
                              , TrainingSet<2> values
                              , double lambda = 0
                              , double alpha = 0.05
                              , double epsilon = 0.001
                              , int    max_iter = 1000
                              );