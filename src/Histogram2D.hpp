//
// Created by mkloczko on 13.03.17.
//

#ifndef CHALLENGE_HISTOGRAM2D_HPP
#define CHALLENGE_HISTOGRAM2D_HPP

#include <algorithm>
#include <cmath>
#include <iostream>
#include <array>

using std::cerr;
using std::endl;
using std::array;

template <int XBins, int YBins>
struct Histogram2D{

    static constexpr double StartX = 0.0;
    static constexpr double EndX   = 1.0;

    static constexpr double StartY = 0.0;
    static constexpr double EndY   = 1.0;

    array<double,XBins*YBins> normalized(){

        array<double,XBins*YBins> ret;
        ret.fill(0);

        if(size > 0){
            for(unsigned long i = 0; i < XBins*YBins; i++){
                ret[i] = (double)bins[i]/(double)size;
            }
        }

        return ret;
    }


    unsigned long bins[XBins*YBins]  = {0};
    unsigned long size =  0;


    void addAt(const double & x, const double & y){
        int which_x = (x - StartX)/((EndX - StartX)/XBins);
        int which_y = (y - StartY)/((EndY - StartY)/YBins);

        which_x = std::max(std::min(XBins-1, which_x), 0);
        which_y = std::max(std::min(YBins-1, which_y), 0);

        size += 1;
        (*this)(x,y) += 1;
    }

    void addRange( const double & x_min, const double & x_max
                 , const double & y_min, const double & y_max){
        int i_x_min = (x_min - StartX)/((EndX - StartX)/XBins);
        int i_x_max = (x_max - StartX)/((EndX - StartX)/XBins);
        int i_y_min = (y_min - StartY)/((EndY - StartY)/YBins);
        int i_y_max = (y_max - StartY)/((EndY - StartY)/YBins);

        i_x_min = std::max(std::min(XBins-1, i_x_min)  ,0);
        i_x_max = std::max(std::min(XBins  , i_x_max+1),1);

        i_y_min = std::max(std::min(YBins-1, i_y_min)  ,0);
        i_y_max = std::max(std::min(YBins  , i_y_max+1),1);

        for( unsigned long y = i_y_min; y < i_y_max; y++){
            for( unsigned long x = i_x_min; x < i_x_max; x++) {
                (*this)(x,y) += 1;
                size +=1;
            }
        }

    }


    unsigned long& operator()( const unsigned long& x
                             , const unsigned long& y){
        return bins[y*XBins + x];
    }

};

template <int X, int Y>
std::ostream& operator<<(std::ostream& os, const Histogram2D<X,Y>& obj)
{
    for(int x = 0; x < X; x++){
        for(int y = 0; y < Y; y++) {
            os << "Bin " << x << "," << y << ": " << obj(x,y) << endl;
        }
    }
    return os;
}

#endif //CHALLENGE_HISTOGRAM2D_HPP
