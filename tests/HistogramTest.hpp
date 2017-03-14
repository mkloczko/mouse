//
// Created by mkloczko on 14.03.17.
//

#ifndef CHALLENGE_HISTOGRAMTEST_HPP
#define CHALLENGE_HISTOGRAMTEST_HPP

#include "../src/Histogram.hpp"

#include <rapidcheck.h>
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <random>
#include <iterator>

using std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::snprintf;
using std::unique_ptr;

template <int B>
void medianTest(){

    string format("Checking whether we get a proper median from a histogram<%d>... ");
    size_t size = snprintf( nullptr, 0, format.c_str(), B ) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), B );

    using namespace rc;
    using namespace rc::gen;
    check(buf.get()
             ,[](){
                Histogram<B> hist;
                const double S = hist.Start;
                const double E = hist.End;

                int no_samples      = *inRange<int>(1,1000);
                vector<double> values(no_samples,0);
//                vector<int> noise_i = *container<vector<int>>(no_samples, inRange<int>(0,99));
                int median_i        = *inRange<int>(0,B);
                int max_l           = *inRange<int>(0, median_i+1);
                int min_r           = *inRange<int>(median_i, B);

                double delta = (E-S)/B;
                double median = median_i * delta;
                unsigned long i = 0;

                //For no_samples == 4 -> i < 1
                //For no_samples == 5 -> i < 2
                for(;i < (no_samples-1)/2; i++){
                    values[i] = *inRange<int>(0, max_l +1);
                    values[i] += ((double)*inRange<int>(0,99))/100.0;
                    values[i] *= delta;
                }
                int first_i = -1;
                int second_i = -1;
                if( no_samples % 2 == 0){
                    //Generate two values which on average give the median value.
                    //TODO
                    first_i = median_i;
                    second_i = median_i;

                    values[i+0]  = first_i;
                    values[i+0] += ((double) *inRange<int>(0,99))/100.0;
                    values[i+0] *= delta;
                    values[i+1]  = second_i;
                    values[i+1] += ((double) *inRange<int>(0,99))/100.0;
                    values[i+1] *= delta;

                    i+=2;
                } else {
                    values[i]  = median_i;
                    values[i] += ((double) *inRange<int>(0,99))/100.0;
                    values[i] *= delta;
                    i+=1;
                }

                for (; i< no_samples; i++){
                    values[i] = *inRange<int>(min_r, B);
                    values[i] += ((double)*inRange<int>(0,99))/100.0;
                    values[i] *= delta;
                }

                std::random_device rd;
                std::mt19937 g(rd());

                std::shuffle(values.begin(), values.end(), g);

                for(const double & v: values){
                    hist += v;
                }

                bool is_ok = (std::abs(median - hist.median()) < 0.001 );

                if(!is_ok){
                    cerr << "Max_l " << max_l << " min_r " << min_r << " median_i " << median_i << endl;
                    cerr << "Generated medians: " << median << " " << hist.median() << endl;
                    if( no_samples %2 == 0){
                        cerr << "The median is a mix of: " << first_i << " " << second_i << endl;
                    }
                    unsigned long elems_below = 0;
                    unsigned long elems_above = 0;
                    std::for_each(values.begin(), values.end()
                                 ,[&elems_below, &elems_above, median](const double & a){
                                if (a > median)
                                    elems_above += 1;
                                if (a < median)
                                    elems_below += 1;
                                return a;
                            });
                    cerr << "Elements above median: " << elems_above << " , elems below: " << elems_below << endl;
                    cerr << hist << endl;
                    cerr << "***" << endl;
                }

                return is_ok;

            });

};

template <int B>
void normalizationTest() {

    string format("Checking whether we get a proper median from a histogram<%d>... ");
    size_t size = snprintf(nullptr, 0, format.c_str(), B) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), B);

    using namespace rc;
    using namespace rc::gen;
    check(buf.get(), []() {
        Histogram<B> hist;

        int no_samples = *inRange<int>(1,1000);

        for (unsigned long i = 0; i < no_samples; i++) {
            double val = *inRange<int>(0, 100);
            hist += val / 100;
        }

        hist.normalize();

        bool is_ok = true;
        for (unsigned long b = 0; b < B && is_ok; b++) {
            is_ok = hist.bins[b] >= 0 && hist.bins[b] <= 1.0;
        }
        double suspected_how_much = 1.0 / no_samples;
        is_ok = is_ok && std::abs(hist.how_much - suspected_how_much) < 0.0001;


        return is_ok;
    });
}

void histogramTestSuite();
#endif //CHALLENGE_HISTOGRAMTEST_HPP
