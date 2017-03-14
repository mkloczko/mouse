//
// Created by mkloczko on 13.03.17.
//

#ifndef CHALLENGE_HISTOGRAM_HPP
#define CHALLENGE_HISTOGRAM_HPP

#include <algorithm>
#include <cmath>
#include <iostream>

using std::cerr;
using std::endl;

template < int Bins>
struct Histogram{

    static constexpr double Start = 0.0;
    static constexpr double End   = 1.0;

    void normalize(){
        unsigned long no_elems = 0;

        how_much = ((double) size )/ (End-Start);

        for(unsigned long i = 0; i < Bins; i++){
            no_elems = (unsigned long)std::ceil(bins[i]/how_much);
            bins[i] = no_elems * how_much;
        }

    }

    double median(){
        unsigned long to_take  = 2 - (size % 2);
        int counter = (size+1)/2;


        //wtf_cpp: -1 if there are two medians (size%2 == 0)
        //          0 if there is one. (size%2 == 1)
        int wtf_cpp = size%2 -1;

        double median = 0;
//        cerr << "Size: " << size << endl;
//        cerr << "Counter: " << counter << endl;
//        cerr << "How much: " << how_much << endl;
        bool first_found = false;
        bool second_found = false;
        for(unsigned long i = 0; i < Bins && counter > wtf_cpp ; i++ ){
            counter -= (unsigned long)std::ceil(bins[i]/how_much);
            if(!first_found && counter <= 0){
                first_found = true;
                median += Start + i * (End - Start);
//                cerr << "Found le first median at " << i << "th bin" << endl;
//                cerr << "Median: " << median << " " << counter << " " << wtf_cpp << endl;
            }
            if(!second_found && size % 2 == 0 && counter <= -1 ) {
                second_found = true;
                median += Start + i * (End - Start);
//                cerr << "Found le second median at " << i << "th bin" << endl;
//                cerr << "Median: " << median << " " << counter << endl;

            }
        }

        return (median/to_take)/Bins;

    }

    double bins[Bins]  = {0};
    unsigned long size =  0;
    double how_much    =  1;

    Histogram& operator+(const double & val){


        int which_bin = std::floor((val - Start)/((End-Start)/Bins));
//        cerr << "And the value " << val << " goes to bin: " << which_bin << ", hm " << how_much << endl;

        which_bin = std::max(std::min(Bins-1, which_bin), 0);
        bins[which_bin] += how_much;
        size += 1;
        return *this;
    }

    Histogram& operator +=(const double & val){
        *this = *this + val;
        return *this;
    }

};

template <int B>
std::ostream& operator<<(std::ostream& os, const Histogram<B>& obj)
{
    for(int i = 0; i < B; i++){
        os << "Bin " << i << ": " << obj.bins[i] << endl;
    }
    return os;
}

template <int Bins>
using ProbHistogram = Histogram<Bins>;

//void addTo(Histogram<Bins> & hist, double which, double max_bin)

//struct Histogram{
//    double bins[10] = {0};
//};

#endif //CHALLENGE_HISTOGRAM_HPP
