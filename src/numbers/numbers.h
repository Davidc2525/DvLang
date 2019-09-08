#ifndef NUMBERS_H
#define NUMBERS_H
#include <limits.h>
#include <iostream>
#include <math.h>
#include <bitset>


namespace NUMBERS
{

class Numbers 
{
  private:
    /* data */
  public:
    Numbers();
    ~Numbers();

    //IEEE 754
    static double ConverToDouble(long input);
    static float ConverToFloat(int input);

    static int FloatToBitInt(float input);
    static long DoubleToBitLong(double input);
    //IEEE 754

    //cast
    /*double to float*/
    static float DoubleToFloat(double input);

    /*float to double*/
    static double FloatToDouble(float input);
    
    /*float to int*/
    static int FloatToInt(float input);
    
    /*double to int*/
    static int DoubleToInt(double input);

};


} // namespace NUMBERS
#endif