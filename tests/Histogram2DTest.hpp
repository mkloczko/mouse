//
// Created by mkloczko on 20.03.17.
//

#ifndef CHALLENGE_HISTOGRAM2DTEST_HPP
#define CHALLENGE_HISTOGRAM2DTEST_HPP
#include "../src/Histogram2D.hpp"

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


template <int X, int Y>
void normalization2DTest() {

    string format("Checking for proper normalization of a histogram2d<%d, %d>... ");
    size_t size = snprintf(nullptr, 0, format.c_str(), X, Y) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), X, Y);

    using namespace rc;
    using namespace rc::gen;
    check(buf.get(), []() {
        Histogram2D<X,Y> hist;

        int no_samples = *inRange<int>(1,10000);

        for (unsigned long i = 0; i < no_samples; i++) {
            double val_x = *inRange<int>(0, 100);
            double val_y = *inRange<int>(0, 100);
            hist.addAt(val_x/100, val_y/100);
        }

        array<double, X*Y> norm = hist.normalized();

        bool is_ok = true;
        double sum = 0;
        unsigned long b = 0;

        for (; b < X*Y && is_ok; b++) {
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


template <int X, int Y>
void rangeNormalization2DTest() {

    string format("Checking for proper normalization of a histogram2d<%d, %d> with ranged insertions... ");
    size_t size = snprintf(nullptr, 0, format.c_str(), X, Y) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), X, Y);

    using namespace rc;
    using namespace rc::gen;
    check(buf.get(), []() {
        Histogram2D<X,Y> hist;

        int no_samples = *inRange<int>(1,10000);

        for (unsigned long i = 0; i < no_samples; i++) {
            double max_x = *inRange<int>(1, 100);
            double max_y = *inRange<int>(1, 100);

            double min_x = *inRange<int>(0, max_x);
            double min_y = *inRange<int>(0, max_y);

            hist.addRange(min_x/100, max_x/100, min_y/100, max_y/100);
        }

        array<double, X*Y> norm = hist.normalized();

        bool is_ok = true;
        double sum = 0;
        unsigned long b = 0;

        for (; b < X*Y && is_ok; b++) {
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

void histogram2DTestSuite();

#endif //CHALLENGE_HISTOGRAM2DTEST_HPP
