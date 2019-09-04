

#include "numbers.h"

using namespace NUMBERS;
 
double Numbers::ConverToDouble(long number)
{
    int mantissaShift = 52;
    unsigned long exponentMask = 0x7FF0000000000000;
    int bias = 1023;
    int signShift = 63;

    int sign = !((number >> signShift) & 0x01);
        sign == 0 ? sign = -1 : sign = 1;
    //bool negative  = !!(number & 0x80000000);
    //int sign = negative ? -1 : 1;
    int exponent = ((number & exponentMask) >> mantissaShift) - bias;

    int power = -1;
    double total = 0.0;
    for (int i = 0; i < mantissaShift; i++)
    {
        int calc = (number >> (mantissaShift - i - 1)) & 0x01;
        total += calc * pow(2.0, power);
        power--;
    }
    total += 1.0;

    double value = sign * pow(2.0, exponent) * total;

    return value;
}

float Numbers::ConverToFloat(int number)
{
    int mantissaShift = 23;
    unsigned long exponentMask = 0x7f800000;
    int bias = 127;
    int signShift = 31;

    int sign = !((number >> signShift) & 0x01);
    sign == 0 ? sign = -1 : sign = 1;
    //bool negative  = !!(number & 0x80000000);
    //int sign = negative ? -1 : 1;
    int exponent = ((number & exponentMask) >> mantissaShift) - bias;

    int power = -1;
    float total = 0.0;
    for (int i = 0; i < mantissaShift; i++)
    {
        int calc = (number >> (mantissaShift - i - 1)) & 0x01;
        total += calc * pow(2.0, power);
        power--;
    }
    total += 1.0;

    float value = sign * pow(2.0, exponent) * total;

    return value;
}

float Numbers::DoubleToFloat(double input) { return (float)input; }

double Numbers::FloatToDouble(float input) { return (double)input; }

int Numbers::FloatToInt(float input) { return (int)input; }

int Numbers::DoubleToInt(double input) { return (int)input; }

// Get 32-bit IEEE 754 format of the decimal value
int Numbers::FloatToBitInt(float value)
{
    union {
        float input; // assumes sizeof(float) == sizeof(int)
        int output;
    } data;

    data.input = value;

    std::bitset<sizeof(float) * CHAR_BIT> bits(data.output);

    /*std::string mystring = bits.to_string<char, 
		                                  std::char_traits<char>,
										  std::allocator<char> >();*/

    return data.output;
}

long Numbers::DoubleToBitLong(double value)
{
    union {
        double input; // assumes sizeof(float) == sizeof(int)
        long output;
    } data;

    data.input = value;

    std::bitset<sizeof(double) * CHAR_BIT> bits(data.output);

    /*std::string mystring = bits.to_string<char, 
		                                  std::char_traits<char>,
										  std::allocator<char> >();*/

    return data.output;
}
