//
// Created by mkloczko on 13.03.17.
//

#ifndef CHALLENGE_WORDDICTIONARY_HPP
#define CHALLENGE_WORDDICTIONARY_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <locale>
#include <tuple>
#include <sstream>
#include <algorithm>

#include <csv.h>

#include "Histogram.hpp"

using std::string;
using std::vector;
using std::set;
using std::tuple;
using std::pair;

//
using WordDictionary = std::map<string, double>;

using WorkingDict    = std::map<string, Histogram<10>>;


const set<string> words_to_ignore
        = { "a"
          , "the"
          , "any"
          , "with"
          , "that"
          , "."
          , ""
          , "?"
          , "!"
          };


/* Divide a sentence into words
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * "catch friendly" -> {"catch","friendly"}
 *
 */
vector<string> sentence_to_words( const string      & sentence
                                , const set<string> & to_ignore = set<string>()){
    string buf;
    std::stringstream ss(sentence);

    vector<string> tokens;
    while (ss >> buf){
        transform( buf.begin(), buf.end(), buf.begin()
                 , [](const char & c){ return std::tolower(c); });
        if (to_ignore.count(buf) == 0)
            tokens.push_back(buf);
    }
    return tokens;
}


void addSentence( const string      & sentence
                , const double      & probability
                ,       WorkingDict & dict){
    vector<string> tokens = sentence_to_words(sentence, words_to_ignore);

    for(const string & token : tokens){
        dict[token] + probability;
    }
}

/* Convert a dictionary of strings and histograms
 * to a dictionary of medians.
 */

WordDictionary to_word_dict(const WorkingDict & dict){
    WorkingDict tmp = dict;
    WordDictionary ret;

    std::transform( dict.begin()   , dict.end()
                  , std::inserter(ret, ret.begin())
                  , [](const pair<string, Histogram<10>> & key_value){
                pair<string, double> median;
                median.first = key_value.first;
                median.second = key_value.second.median();
                return median;
            } );

    return ret;
}

/* Load a simple dataset
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * ```
 * sentence                   ; animal_prob
 * I care a lot about my mouse; 1.0
 * ```
 */
vector<tuple<string,double>> loadSimpleDataset(const string & dataset){
    io::CSVReader<2
                 , io::trim_chars<' ', '\t'>
                 , io::no_quote_escape<';'>> in(dataset);

    vector<tuple<string, double>> ret;

    in.read_header(io::ignore_extra_column, "sentence", "animal_prob");
    std::string sentence; double probability;

    int i = 0;
    while(in.read_row(sentence, probability)){
        ret.push_back(std::make_tuple(sentence, probability));
        i++;
    }
    return ret;
};

/* Load a complex dataset
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * ```
 * sentence                   ; animal_words ; animal_prob ; computer_words ; computer_prob
 * I care a lot about my mouse; care         ; 1.0         ;                ; 0.0
 * ```
 */

vector<tuple<string,string,double,string,double>> loadComplexDataset(const string & dataset){
    io::CSVReader<5
            , io::trim_chars<' ', '\t'>
            , io::no_quote_escape<';'>> in(dataset);

    vector<tuple<string, string,double,string,double>> ret;

    in.read_header(io::ignore_extra_column, "sentence"
                  ,"animal_words"  , "animal_prob"
                  ,"computer_words", "computer_prob");

    std::string sentence;
    std::string animal_words;   double animal_prob;
    std::string computer_words; double computer_prob;

    int i = 0;
    while(in.read_row(sentence, animal_words, animal_prob, computer_words, computer_prob)){
        ret.push_back(std::make_tuple( sentence, animal_words, animal_prob
                                     , computer_words, computer_prob));
        i++;
    }
    return ret;
};

tuple<WordDictionary, WordDictionary> complexDatasetToDicts(
        const vector<tuple<string, string,double,string,double>> & vec){

    WorkingDict work_animal;
    WorkingDict work_computer;

    for(auto & el : vec){
        addSentence(std::get<1>(el), std::get<2>(el), work_animal);
        addSentence(std::get<3>(el), std::get<4>(el), work_computer);
    }

    return make_tuple(to_word_dict(work_animal), to_word_dict(work_computer));

};

WordDictionary simpleDatasetToDict(const vector<tuple<string,double>> & vec){
    WorkingDict work;

    for(const tuple<string,double> & el : vec){
        addSentence(std::get<0>(el), std::get<1>(el), work);
    }

    return to_word_dict(work);
}

#endif //CHALLENGE_WORDDICTIONARY_HPP
