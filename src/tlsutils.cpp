#include "tlsutils.h"

double roundto( double value, int precision )
{
    //const int adjustment = pow(10,precision);  //works, but not on windows?
    const int adjustment = pow((double)10,precision);
    return floor( value*(adjustment) + 0.5 )/adjustment;
}