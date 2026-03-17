#include "vocal_processor.h"
#include "output_tee_globals.h"

#include <string>
#include <iostream>

using std::endl;
using std::string;

void Vocal_processor::initialize()
{
    Motor_processor::initialize();
    //	set_randomize_when_used(true);
    //	set_randomize(true);
    //	randomize();
    //	describe_parameters(Normal_out);
}