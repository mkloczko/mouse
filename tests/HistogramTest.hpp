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
#include <array>

using std::vector;
using std::cerr;
using std::endl;
using std::string;
using std::snprintf;
using std::unique_ptr;
using std::array;

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

                int no_samples      = *inRange<int>(1,10000);
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
                    int max_range = std::min(median_i - max_l, min_r - median_i);

                    first_i = *inRange<int>(median_i - max_range, median_i + max_range + 1);

                    second_i = 2*median_i - first_i;

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
                    unsigned long elems_equal = 0;
                    std::for_each(values.begin(), values.end()
                                 ,[&elems_below, &elems_above, &elems_equal, median]
                                  (const double & a){
                                if (a > median)
                                    elems_above += 1;
                                else if (a < median)
                                    elems_below += 1;
                                else
                                    elems_equal += 1;
                                return a;
                            });
                    cerr << "Elements above median: " << elems_above
                         << ", elements below: " << elems_below
                         << ", elements equal: " << elems_equal << endl;
                    cerr << hist << endl;
                    cerr << "***" << endl;
                }

                return is_ok;

            });

};

template <int B>
void normalizationTest() {

    string format("Checking for proper normalization of a histogram<%d>... ");
    size_t size = snprintf(nullptr, 0, format.c_str(), B) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), B);

    using namespace rc;
    using namespace rc::gen;
    check(buf.get(), []() {
        Histogram<B> hist;

        int no_samples = *inRange<int>(1,10000);

        for (unsigned long i = 0; i < no_samples; i++) {
            double val = *inRange<int>(0, 100);
            hist += val / 100;
        }

        array<double, B> norm = hist.normalized();

        bool is_ok = true;
        double sum = 0;
        unsigned long b = 0;

        for (; b < B && is_ok; b++) {
            sum += norm[b];
            is_ok = norm[b] >= 0 && norm[b] <= 1.0;
        }
        if (!is_ok){
            cerr << "Bin " << b << "  was not in 0.0,1.0 range: " << hist.bins[b] << endl;
        }


        if (is_ok && std::abs(sum - 1.0) > 0.0001){
            is_ok = false;
            cerr << "Total sum of bins is not equal to 1.0: " << sum << endl;
        }

        return is_ok;
    });
}

void histogramTestSuite();
#endif //CHALLENGE_HISTOGRAMTEST_HPP
