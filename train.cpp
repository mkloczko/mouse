#include <iostream>
#include <random>
#include <fstream>


#include "src/Histogram.hpp"
#include "src/NeuralNetwork.hpp"
#include "src/WordDictionary.hpp"

using namespace std;


Layer<11> toInput(const string & sentence, const WordDictionary & dict){
    Layer<11>     ret  = Layer<11>::Zero();
    Histogram<10> hist;

    vector<string> words = sentence_to_words(sentence);

    for(unsigned int i = 0; i < words.size(); i++){
        try{
            hist += dict.at(words[i]);
        } catch (const std::out_of_range & e){
//            hist += 0.5;
            //Try to guess, using a smaller neural network ?
            //TODO.
        }
    }

    array<double,10> norm = hist.normalized();

    for(unsigned int i = 0; i < 10; i++){
        ret(i) = norm[i];
    }

    ret(10) = ((double)sentence.size())/200.0;

    return ret;
}

tuple<TrainingSet<11>, TrainingSet<1>> neuralFood
        ( const vector<tuple<string, double>> & dataset
        , const WordDictionary                & dict){

    TrainingSet<11> inputs(11,dataset.size());
    TrainingSet<1>  values(1 ,dataset.size());

    for(unsigned long i = 0; i < dataset.size(); i++) {
        inputs.col(i) = toInput(get<0>(dataset[i]), dict);
        values(i)     = get<1>(dataset[i]);
    }

    return std::tie(inputs,values);
}



int main(int argc, const char ** argv) {


    string filename("dataset/sample.csv");
    string trained("trained/sample11-12-1.nn");

    if (argc >= 2){
        filename = string(argv[1]);
    }
    if (argc >= 3){
        trained = string(argv[2]);
    }


    using Dataset = vector<tuple<string,double>>;
    Dataset dataset;
    try {
        dataset = loadSimpleDataset(filename);
    } catch(io::error::can_not_open_file e) {
        cerr << e.what() << endl;
        return 1;
    } catch(io::error::line_length_limit_exceeded e){
        cerr << e.what() << endl;
        return 2;
    } catch(io::error::missing_column_in_header e){
        cerr << e.what() << endl;
        return 3;
    } catch(io::error::too_few_columns e){
        cerr << e.what() << endl;
        return 4;
    } catch(io::error::too_many_columns e){
        cerr << e.what() << endl;
        return 5;
    }

    /* Training, evaluation, and dictionary sets.
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *
     * The data set is split into two parts - one for
     * dictionary creation, and another one for the
     * machine learning purposes. These parts may overlap,
     * but it's important to test the algorithm on non-overlapping
     * section to see how the algorithm deals with new words.
     *
     * In the future we plan to introduce the cross-validation dataset.
     *
     */

    random_device rd;
    mt19937 g(rd());

    shuffle(dataset.begin(), dataset.end(), g);



    Dataset dictionary_dataset = Dataset( dataset.begin()
                                        , dataset.end() // - dataset.size()/2
                                        );

    Dataset training_dataset   = Dataset( dataset.begin() // + dataset.size()/4
                                        , dataset.end()   // - dataset.size()/4*3
                                        );
    Dataset evaluation_dataset = Dataset( dataset.begin() // + dataset.size()/4*3
                                        , dataset.end()
                                        );


    WordDictionary dict = simpleDatasetToDict(dictionary_dataset);

    tuple<TrainingSet<11>, TrainingSet<1>> training_food   = neuralFood(training_dataset  , dict);
    tuple<TrainingSet<11>, TrainingSet<1>> evaluation_food = neuralFood(evaluation_dataset, dict);





    NeuralNetwork<11, 12, 1> network;

    NeuralNetwork<11,12,1> learned = gradient_descent(network, get<0>(training_food), get<1>(training_food)
                    ,0, 0.05, 0.0000001, 10000);


    double cost_training   = costFunction(network, get<0>(training_food)  , get<1>(training_food));
    double cost_evaluation = costFunction(learned, get<0>(evaluation_food), get<1>(evaluation_food));

    cout << cost_training << ", " << cost_evaluation << endl;

    std::ofstream file(trained);
    if (file.is_open())
    {
//        file << learned.weights_0 << "\n" << learned.weights_1 << endl;
        for(unsigned long i = 0; i < network.weights_0.size(); i++){
            file << learned.weights_0(i) << " ";
        }
        file << endl;
        for(unsigned long i = 0; i < network.weights_1.size(); i++){
            file << learned.weights_1(i) << " ";
        }
        file << endl;
        std::for_each(dict.begin(), dict.end(), [&](const pair<string, double>& k_v){
            file << k_v.first << " " << k_v.second << endl;
            return k_v;
        });
    }

    file.close();

#ifdef DEBUG
    std::for_each(dict.begin(), dict.end(), [](const pair<string, double> & pair){
        cout << pair.first << ": " << pair.second << endl;
        return pair;
    });


    for (unsigned long i = 0; i < evaluation_dataset.size(); i++){
        Layer<11> input  = toInput(get<0>(evaluation_dataset[i]), dict);
        Layer<1>  output = learned.compute(input);
        cout << i << ": " << get<0>(evaluation_dataset[i]) << endl;
        cout << input.transpose() <<" -> " << output.transpose() << " vs "
                                           << get<1>(evaluation_dataset[i]) << endl;
    }
#endif


    return 0;
}