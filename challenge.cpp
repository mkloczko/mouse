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

    // Load the trained neural network.
    NeuralNetwork<11,12,1> network;
    WordDictionary dict;

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
            dict[key] = value;
        }

    }

    file.close();


    for (unsigned long i = 0; i < dataset.size(); i++){
        Layer<11> input  = toInput(get<0>(dataset[i]), dict);
        Layer<1>  output = network.compute(input);
        if(output(0) > 0.5)
            cout << "animal" << endl;
        else
            cout << "computer" << endl;
    }



    return 0;
}