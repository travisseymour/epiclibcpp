// global output objects
#include "output_tee_globals.h"
#include "output_tee.h"
#include "pps_globals.h"

Output_tee Normal_out;
Output_tee Trace_out;
Output_tee Debug_out;
Output_tee Device_out;
Output_tee Exception_out;
Output_tee Stats_out;
Output_tee Info_out;

void cleanup_all_output_tees()
{
#ifdef EPICLIB_WITH_PYBIND11
    Normal_out.clear_py_streams();
    Trace_out.clear_py_streams();
    Debug_out.clear_py_streams();
    Device_out.clear_py_streams();
    Exception_out.clear_py_streams();
    Stats_out.clear_py_streams();
    Info_out.clear_py_streams();
    PPS_out.clear_py_streams();
#endif
}