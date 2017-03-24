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

    // Load the trained neural network.
    NeuralNetwork<201,50,2> network;
    WordDictionary dict_animal;
    WordDictionary dict_computer;

    std::ifstream file(trained);
    if (file.is_open()){
        for(unsigned long i = 0; i < network.weights_0.size(); i++){
            file >> network.weights_0(i);
        }
        for(unsigned long i = 0; i < network.weights_1.size(); i++){
            file >> network.weights_1(i);
        }
        string key;
        double value;
        while(file >> key >> value){
            dict_animal[key] = value;
        }

        //Check for the divider.
        string uh;
        file.clear();
        getline(file, uh);

        while(file >> key >> value){
            dict_computer[key] = value;
        }


    }

    file.close();


    for (unsigned long i = 0; i < dataset.size(); i++){
        Layer<201> input  = toInput(get<0>(dataset[i]), tie(dict_animal, dict_computer));
        Layer<2>  output = network.compute(input);
        if(output(0) > output(1))
            cout << "animal" << endl;
        else
            cout << "computer" << endl;
    }



    return 0;
}