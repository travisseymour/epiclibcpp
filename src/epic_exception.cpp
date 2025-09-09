#include "epic_exception.h"
#include "processor.h"

Epic_exception::Epic_exception(Processor * proc_ptr, const std::string& msg_)
{
	msg = proc_ptr->processor_info() + " " + msg_;
}