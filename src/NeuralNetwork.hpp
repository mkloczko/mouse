#include <iostream>
#include <exception>
#include <Eigen/Core>
#include <cmath>

using std::cerr;
using std::endl;

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
template <int Input, int Middle, int Output>
struct NeuralNetwork{

    Weights<Middle, Input+1> weights_0 = 0.001  * Weights<Middle, Input+1>::Random();
    Weights<Output, Middle+1> weights_1 = 0.001 * Weights<Output, Middle+1>::Random();

    Layer<Output>       compute(Layer<Input>       input) const{
        return forwardProp(forwardProp(input, weights_0), weights_1);
    }

    TrainingSet<Output> compute(TrainingSet<Input> input) const{
        return forwardProp(forwardProp(input, weights_0), weights_1);
    }
};

//The function has to be numerically robust.
template <int I, int M, int O>
double costFunction( const NeuralNetwork<I,M,O> & network
                   , TrainingSet<I> training
                   , TrainingSet<O> values
                   , double lambda = 0) {
    //Cost function
    auto hs    = network.compute(training);
    auto part1 = values.array() * hs.array().log() ;
    auto part2 = (1 - values.array()) * (1 - hs.array()).array().log();
    auto elems = - (part1 + part2);

    double sum = elems.sum()/training.cols();

    //Regularisation
    double reg = 0;
    reg += network.weights_0.template rightCols<I>().unaryExpr([](double a){return a * a;}).sum();
    reg += network.weights_1.template rightCols<M>().unaryExpr([](double a){return a * a;}).sum();
    reg = reg/(2*training.cols());
    reg = reg * lambda;

    return sum + reg;
}

template <int I, int M, int O>
NeuralNetwork<I,M,O> backpropagation(const NeuralNetwork<I,M,O> & network,
                              TrainingSet<I> training,
                              TrainingSet<O> values,
                              double lambda = 0){
    if (training.cols() != values.cols()){
        throw std::invalid_argument("The number of X's is not equal to number of Y's.");
    }

    Weights<M, I+1> delta_0 = Weights<M,I+1>::Zero();
    Weights<O, M+1> delta_1 = Weights<O,M+1>::Zero();

    Layer<I> in_layer;
    Layer<M> mid_layer;
    Layer<O> out_layer;

    Layer<O> out_error;
    Layer<M> mid_error;

    for(unsigned long i = 0; i < training.cols(); i++){
        in_layer  = training.col(i);
        mid_layer = forwardProp(in_layer,  network.weights_0);
        out_layer = forwardProp(mid_layer, network.weights_1);

        out_error = out_layer - values.col(i);
        mid_error = (network.weights_1.transpose() * out_error ).template bottomRows<M>()
                            .array() * mid_layer.array();
        mid_error = mid_error.array() * (mid_layer.array() * (-1) + 1);


        delta_0 += mid_error*addBias(in_layer).transpose();
        delta_1 += out_error*addBias(mid_layer).transpose();
    }

    //Add the regularisation parameter
    delta_0.template rightCols<I>() += lambda*network.weights_0.template rightCols<I>();
    delta_1.template rightCols<M>() += lambda*network.weights_1.template rightCols<M>();

    //Divide by the number of training samples
    delta_0 /= training.cols();
    delta_1 /= training.cols();

    NeuralNetwork<I,M,O> ret;
    ret.weights_0 = delta_0;
    ret.weights_1 = delta_1;

    return ret;
}

//Calculates the derivative of the cost function based on numerical derivatives
template <int I, int M, int O>
NeuralNetwork<I,M,O> numerical_derivative( const NeuralNetwork<I,M,O> & thetas
                                  , TrainingSet<I> training
                                  , TrainingSet<O> values
                                  , double lambda = 0
                                  , double epsilon= 0.001){

    NeuralNetwork<I,M,O> ret;
    //Need to traverse through all Thetas.
    for(unsigned long i = 0; i < thetas.weights_0.size(); i++){
        NeuralNetwork<I,M,O> thetas_plus  = thetas;
        NeuralNetwork<I,M,O> thetas_minus = thetas;
        thetas_plus .weights_0(i) += epsilon;
        thetas_minus.weights_0(i) -= epsilon;

        double J_plus  = costFunction(thetas_plus , training, values, lambda);
        double J_minus = costFunction(thetas_minus, training, values, lambda);

        ret.weights_0(i) = (J_plus - J_minus)/(2*epsilon);
    }

    for(unsigned long i = 0; i < thetas.weights_1.size(); i++){
        NeuralNetwork<I,M,O> thetas_plus  = thetas;
        NeuralNetwork<I,M,O> thetas_minus = thetas;
        thetas_plus .weights_1(i) += epsilon;
        thetas_minus.weights_1(i) -= epsilon;

        double J_plus  = costFunction(thetas_plus , training, values, lambda);
        double J_minus = costFunction(thetas_minus, training, values, lambda);

        ret.weights_1(i) = (J_plus - J_minus)/(2*epsilon);
    }

    return ret;
}

template <int I, int M, int O>
NeuralNetwork<I,M,O> gradient_descent( const NeuralNetwork<I,M,O> & network
                              , TrainingSet<I> training
                              , TrainingSet<O> values
                              , double lambda  = 0
                              , double alpha   = 0.05
                              , double epsilon = 0.001
                              , int    max_iter= 1000){
    NeuralNetwork<I,M,O> prev    = network;
    NeuralNetwork<I,M,O> current = network;
    NeuralNetwork<I,M,O> diffs  ;
    NeuralNetwork<I,M,O> thetas  = backpropagation(current, training, values, lambda);
    current.weights_0 -= alpha*thetas.weights_0;
    current.weights_1 -= alpha*thetas.weights_1;


    unsigned long ix = 0;
    bool converged = true;
    converged = converged && (current.weights_0 - prev.weights_0).array().abs().maxCoeff() < epsilon;
    converged = converged && (current.weights_1 - prev.weights_1).array().abs().maxCoeff() < epsilon;

    while(ix < max_iter && !converged ){
        prev = current;
        thetas = backpropagation(current, training, values, lambda);

        current.weights_0 -= alpha*thetas.weights_0;
        current.weights_1 -= alpha*thetas.weights_1;

        converged = true;
        converged = converged && (current.weights_0 - prev.weights_0).array().abs().maxCoeff() < epsilon;
        converged = converged && (current.weights_1 - prev.weights_1).array().abs().maxCoeff() < epsilon;

        ix +=1;
    }

    return current;
}