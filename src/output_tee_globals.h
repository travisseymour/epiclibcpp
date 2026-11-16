#ifndef OUTPUT_TEE_GLOBALS_H
#define OUTPUT_TEE_GLOBALS_H

#include "output_tee.h"

// global output objects
extern Output_tee Normal_out;
extern Output_tee Trace_out;
extern Output_tee Debug_out;
extern Output_tee Device_out;
extern Output_tee Exception_out;
extern Output_tee Stats_out;
extern Output_tee Info_out;

// Cleanup function to clear all Python streams from all global Output_tee instances.
// This MUST be called from Python before the interpreter shuts down to prevent
// segfaults caused by py::object references being destroyed after Python cleanup.
void cleanup_all_output_tees();

#endif