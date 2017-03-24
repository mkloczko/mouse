#include <iostream>
#include <random>
#include <fstream>


#include "src/Histogram.hpp"
#include "src/Histogram2D.hpp"
#include "src/NeuralNetwork.hpp"
#include "src/WordDictionary.hpp"

using namespace std;


Layer<201> toInput(const string & sentence, const tuple<WordDictionary,WordDictionary> & dicts){
    Layer<201>     ret  = Layer<201>::Zero();
    Histogram2D<10,10> hist_animal;
    Histogram2D<10,10> hist_computer;

    vector<string> words = sentence_to_words(sentence);


    for(unsigned int i = 0; i < words.size(); i++){
        try{
            double prob = get<0>(dicts).at(words[i]);
            hist_animal.addRange( prob, prob
                                , i/((double)words.size())
                                , i/((double)words.size()+1)
                                );
        } catch (const std::out_of_range & e){
            //Try to guess, using a smaller neural network ?
            //TODO.
        }

        try{
            double prob = get<1>(dicts).at(words[i]);
            hist_computer.addRange( prob, prob
                    , i/((double)words.size())
                    , i/((double)words.size()+1)
            );
        } catch (const std::out_of_range & e){
            //Try to guess, using a smaller neural network ?
            //TODO.
        }

    }

    array<double,100> norm_animal   = hist_animal.normalized();
    array<double,100> norm_computer = hist_computer.normalized();

    for(unsigned int i = 0; i < 100; i++){
        ret(i)     = norm_animal[i];
        ret(100+i) = norm_computer[i];
    }

    ret(200) = ((double)sentence.size())/200.0;

    return ret;
}

tuple<TrainingSet<201>, TrainingSet<2>> neuralFood
        ( const vector<tuple<string,string,double,string, double>> & dataset
        , const tuple<WordDictionary,WordDictionary>               & dicts){

    TrainingSet<201> inputs(201,dataset.size());
    TrainingSet<2>   values(2  ,dataset.size());

    for(unsigned long i = 0; i < dataset.size(); i++) {
        inputs.col(i) = toInput(get<0>(dataset[i]), dicts);
        values(0,i)   = get<2>(dataset[i]);
        values(1,i)   = get<4>(dataset[i]);
    }

    return std::tie(inputs,values);
}



int main(int argc, const char ** argv) {


    string filename("dataset/sample_complex.csv");
    string trained("trained/sample_complex201-50-2.nn");

    if (argc >= 2){
        filename = string(argv[1]);
    }
    if (argc >= 3){
        trained = string(argv[2]);
    }


    using Dataset = vector<tuple<string,string,double,string,double>>;
    Dataset dataset;
    try {
        dataset = loadComplexDataset(filename);
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

    tuple<WordDictionary,WordDictionary> dicts = complexDatasetToDicts(dictionary_dataset);

    tuple<TrainingSet<201>, TrainingSet<2>> training_food   = neuralFood(training_dataset  , dicts);
    tuple<TrainingSet<201>, TrainingSet<2>> evaluation_food = neuralFood(evaluation_dataset, dicts);




    NeuralNetwork<201, 50, 2> network;

    NeuralNetwork<201,50,2> learned = gradient_descent(network, get<0>(training_food), get<1>(training_food)
                    ,0, 0.05, 0.00001, 5000);


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
        std::for_each(get<0>(dicts).begin(), get<0>(dicts).end(), [&](const pair<string, double>& k_v){
            file << k_v.first << " " << k_v.second << endl;
            return k_v;
        });
        file << endl;
        file << "SEPARATING DICTIONARIES" << endl;
        std::for_each(get<1>(dicts).begin(), get<1>(dicts).end(), [&](const pair<string, double>& k_v){
            file << k_v.first << " " << k_v.second << endl;
            return k_v;
        });
        file << endl;
    }

    file.close();

#ifdef DEBUG
    cout << "Animal dictionary: " << endl;
    std::for_each(get<0>(dicts).begin(), get<0>(dicts).end(), [](const pair<string, double> & pair){
        cout << pair.first << ": " << pair.second << endl;
        return pair;
    });

    cout << "Computer dictionary: " << endl;
    std::for_each(get<1>(dicts).begin(), get<1>(dicts).end(), [](const pair<string, double> & pair){
        cout << pair.first << ": " << pair.second << endl;
        return pair;
    });


    for (unsigned long i = 0; i < evaluation_dataset.size(); i++){
        Layer<201> input = toInput(get<0>(evaluation_dataset[i]), dicts);
        Layer<2>  output = learned.compute(input);
        cout << i << ": " << get<0>(evaluation_dataset[i]) << endl;
        cout << /* input.transpose() <<" -> " <<*/ output.transpose() << " vs "
                                           << get<2>(evaluation_dataset[i])
                                           << ", "
                                           << get<4>(evaluation_dataset[i])
                                           << endl;
    }
#endif


    return 0;
}