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

#include "Histogram.hpp"

using std::string;
using std::vector;
using std::set;
using std::tuple;
using std::pair;

//
using WordDictionary = std::map<string, double>;

using WorkingDict    = std::map<string, ProbHistogram<10>>;


static constexpr set<string> words_to_ignore
        = { "a"
          , "the"
          , "any"
          , "with"
          , "that"
          , "."
          , ""
          , "?"
          };

vector<string> sentence_to_words( const string      & sentence
                                , const set<string> & to_ignore = set<string>()){
    string buf;
    std::stringstream ss(sentence);

    vector<string> tokens;
    while (ss >> buf){
        std::transform(buf.begin(), buf.end(), buf.begin(), std::tolower);
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

WordDictionary to_word_dict(const WorkingDict & dict){
    WorkingDict tmp = dict;
    WordDictionary ret;

    std::transform( dict.begin()   , dict.end()
                  , std::inserter(ret, ret.begin())
                  , [](const pair<string, ProbHistogram<10>> & key_value){
                pair<string, double> median;
                median.first = key_value.first;
                median.second = key_value.second.median();
                return median;
            } );

    return ret;
}

#endif //CHALLENGE_WORDDICTIONARY_HPP
