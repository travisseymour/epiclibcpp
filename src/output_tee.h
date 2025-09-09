#ifndef OUTPUT_TEE_H
#define OUTPUT_TEE_H

#include "view_base.h"
#include "assert_throw.h"
#include <iostream>
#include <sstream>
#include <list>

// --- PYTHON INTEGRATION (guarded) ---------------------------------
#ifdef EPICLIB_WITH_PYBIND11
  #include <pybind11/pybind11.h>
  #include <pybind11/pytypes.h>
  namespace py = pybind11;
#endif
// ------------------------------------------------------------------

/*

The Output_tee class allows you to split output among as many output streams
and View_bases as you wish.  It keeps a list of pointers to ostream objects and View_bases,
and overloads operator<< to output to each stream in the list, and to an internal
ostringstring object whose contents are written a line-at-a-time to each View_base.

This class assumes the pointed-to stream objects and View_bases exist.
If the stream object or View_base is deallocated, it should first be taken out of the list.
No check is made for putting the same stream or View_base in the list more than once.

ostringstream object is always present, but is only written to or manipulated if
there are View_bases in the list.
*/


class Output_tee {
public:
	// return true if the list of streams or View_bases is non-empty;
	// if the list of streams is non-empty, at least the first must be good
	// usage:
	// if(Normal_out)
	//	Normal_out << stuff;

//	operator bool() const
//		{
//			return (
//				(!stream_ptr_list.empty() && stream_ptr_list.front()->good())
//			 	||
//			 	!view_ptr_list.empty());
//		}

	// return true if we have any active sinks: C++ streams, views, or Python streams
	operator bool() const
	{
		if (!stream_ptr_list.empty() && stream_ptr_list.front()->good())
			return true;
		if (!view_ptr_list.empty())
			return true;
#ifdef EPICLIB_WITH_PYBIND11
		if (!py_streams.empty())
			return true;
#endif
		return false;
	}

	// put a stream into the list
	void add_stream(std::ostream& os)
		{
			std::ostream * os_ptr = &os;
			stream_ptr_list.push_back(os_ptr); 
		}

	// take a stream out of the list		
	void remove_stream(std::ostream& os)
		{
			stream_ptr_list.remove(&os);
		}
	// is a stream in the list
	bool is_present(std::ostream& os)
		{
			for(stream_ptr_list_t::iterator it = stream_ptr_list.begin(); it != stream_ptr_list.end(); ++it)
				if(*it == &os)
					return true;
			return false;
		}

	// put an View_base into the list
	void add_view(View_base * view_ptr)
		{
			view_ptr_list.push_back(view_ptr); 
		}

	// take an View_base out of the list		
	void remove_view(View_base * view_ptr)
		{
			view_ptr_list.remove(view_ptr);
		}

	// is a view in the list
	bool is_present(View_base * view_ptr)
		{
			for(view_ptr_list_t::iterator it = view_ptr_list.begin(); it != view_ptr_list.end(); ++it)
				if(*it == view_ptr)
					return true;
			return false;
		}

	// check if there are stream pointers in the stream pointer list - or window pointers - if so, output being produced
//	bool output_enabled()
//		{
//			return !stream_ptr_list.empty() || !view_ptr_list.empty();
//		}

	bool output_enabled()
	{
		if (!stream_ptr_list.empty())
			return true;
		if (!view_ptr_list.empty())
			return true;
#ifdef EPICLIB_WITH_PYBIND11
		if (!py_streams.empty())
			return true;
#endif
		return false;
	}

    // templated member function applies for any type being output
    void write (std::string x)
    {
        std::ostream * os_ptr;
        for (stream_ptr_list_t::iterator it = stream_ptr_list.begin(); it != stream_ptr_list.end(); it++) {
            os_ptr = *it;
            if ((*os_ptr).good())
                *os_ptr << x;
        }

#ifdef EPICLIB_WITH_PYBIND11
		write_to_python_streams(x);
#endif

        // do nothing further if no views in the list
        if(view_ptr_list.empty())
            return;

        view_line_buffer << x;

        return;
    }

	// templated member function applies for any type being output
	template <class T>
	Output_tee& operator<< (const T& x)
	{
		std::ostream * os_ptr;
		for (stream_ptr_list_t::iterator it = stream_ptr_list.begin(); it != stream_ptr_list.end(); it++) {
			os_ptr = *it;
			if ((*os_ptr).good())
				*os_ptr << x;
			}

#ifdef EPICLIB_WITH_PYBIND11
		std::ostringstream __oss;
		if (!stream_ptr_list.empty())
			__oss.copyfmt(*stream_ptr_list.front());  // match flags/precision/width
		__oss << x;
		write_to_python_streams(__oss.str());  // to avoid this formatting/precision, just use std::ostringstream oss
#endif

		// do nothing further if no views in the list
		if(view_ptr_list.empty())
			return *this;
		
		view_line_buffer << x;

		return *this;
	}

	// specialize for manipulators defined on ios_base
	Output_tee& operator<< (std::ios_base& (*manip) (std::ios_base&))
	{
		std::ostream * os_ptr;
		for (stream_ptr_list_t::iterator it = stream_ptr_list.begin(); it != stream_ptr_list.end(); it++) {
			os_ptr = *it;
			if ((*os_ptr).good())
				manip(*os_ptr);
			}

		// do nothing further if no views in the list
		if(view_ptr_list.empty())
			return *this;

		os_ptr = &view_line_buffer;
		manip(*os_ptr);

		return *this;	
	}

	// specialize for manipulators defined on ostream
	Output_tee& operator<< (std::ostream& (*manip) (std::ostream&))
	{
		// 1) Apply manipulator to all attached C++ streams
		for (auto* os_ptr : stream_ptr_list) {
			if (os_ptr && os_ptr->good())
				manip(*os_ptr);
		}

#ifdef EPICLIB_WITH_PYBIND11
	// --- Portable mirroring to Python (no pointer equality) ---
	// Probe what the manipulator writes (endl -> "\n", flush -> "")
	std::ostringstream __probe;
	manip(__probe);
	const std::string __probe_text = __probe.str();

	if (!__probe_text.empty()) {
		// forward exactly what the manipulator wrote (typically "\n")
		write_to_python_streams(__probe_text);
	}
	// endl implies a flush; flush() is a no-op for data, but we always flush Python here
	flush_python_streams();
#endif

	// If no views, we’re done (Python already handled above)
	if (view_ptr_list.empty())
		return *this;

	// 2) Apply manipulator to the view line buffer
	manip(view_line_buffer);

	// 3) If newline, deliver full line to views and reset
	const std::string& s = view_line_buffer.str();
	if (!s.empty() && s.back() == '\n') {
		for (auto* v : view_ptr_list)
			v->notify_append_text(view_line_buffer.str());
		view_line_buffer.str("");
		view_line_buffer.clear();
	}
	return *this;
}


#ifdef EPICLIB_WITH_PYBIND11
	// Attach/detach Python streams (objects with write/flush/close)
	void add_py_stream(py::object stream) {
		// Store a strong ref; allow duplicates only if you want fan-out duplication
		py_streams.push_back(std::move(stream));
	}
	// Optional niceties — use only if you want to support detaching/clearing
	bool remove_py_stream(const py::object& stream) {
		py::gil_scoped_acquire gil;   // safe even if already held
		for (auto it = py_streams.begin(); it != py_streams.end(); ++it) {
			if (it->is(stream)) {     // identity check: Python's "is"
				py_streams.erase(it);
				return true;
			}
		}
		return false;
	}

	void clear_py_streams() { py_streams.clear(); }

	// Convenience for Python: Normal_out("text") and Normal_out.py_write("text")
	void py_write(const std::string& s) {
		// Reuse the standard << path so formatting/newline behavior stays identical
		(*this) << s;
	}
	void py_flush() {
		flush_python_streams();
		// if you have a native flush() in Output_tee, call it too:
		// this->flush();
	}
	void py_close() {
		close_python_streams();
	}
#endif // EPICLIB_WITH_PYBIND11

	friend class Output_tee_format_saver;

private:
	typedef std::list<std::ostream *> stream_ptr_list_t;
	stream_ptr_list_t stream_ptr_list;
	typedef std::list<View_base *> view_ptr_list_t;
	view_ptr_list_t view_ptr_list;
	std::ostringstream view_line_buffer;

#ifdef EPICLIB_WITH_PYBIND11
	std::list<py::object> py_streams;

	// Write a chunk (exactly as sent to std::ostreams) to all Python streams
	void write_to_python_streams(const std::string& chunk) {
		if (py_streams.empty())
			return;
		// We may be called from C++ code while in Python land; always (re)acquire the GIL
		py::gil_scoped_acquire gil;
		for (auto& obj : py_streams) {
			try {
				obj.attr("write")(chunk);
			} catch (const py::error_already_set& e) {
				// Swallow to keep C++ logging robust; optionally you could route to stderr.
				PyErr_Clear();
			}
		}
	}
	void flush_python_streams() {
		if (py_streams.empty())
			return;
		py::gil_scoped_acquire gil;
		for (auto& obj : py_streams) {
			if (py::hasattr(obj, "flush")) {
				try { obj.attr("flush")(); }
				catch (const py::error_already_set&) { PyErr_Clear(); }
			}
		}
	}
	void close_python_streams() {
		if (py_streams.empty())
			return;
		py::gil_scoped_acquire gil;
		for (auto& obj : py_streams) {
			if (py::hasattr(obj, "close")) {
				try { obj.attr("close")(); }
				catch (const py::error_already_set&) { PyErr_Clear(); }
			}
		}
	}
#endif // EPICLIB_WITH_PYBIND11

};

// This class provides for saving and restoring the stream formatting flags.
// Instantiating it saves the current state of the stream formatting in the Output_tee
// and then restores it when destructed. Use in any function that alters the stream formatting.
// It is assumed that the formatting for the first stream or the internal ostreamstringg 
// applies to all streams; if no streams or views are associated with the Output_tee at 
// the time of construction, the default settings are saved. The settings of the first stream
// are saved; if no streams, then the settings of the internal stringstream are saved.
// At the time of destruction, the streams or views will be set to the saved settings.
// So for consistency and clarity, the number of streams or views associated with the Output_tee 
// should not be altered during the lifetime of an Output_tee_format_saver.


// Usage example:
//	void func(Output_tee& ot)
//	{
//		Output_tee_format_saver s(ot);
//		ot << fixed << setprecision(2) << x << endl;
//	}	// state of ot restored at exit

class Output_tee_format_saver {
public:
	Output_tee_format_saver(Output_tee& output_tee_) :
		output_tee(output_tee_), old_flags(std::ios::fmtflags()), old_precision(0)
		{
			std::ostream * os_ptr = 0;
			// if there is an output stream, save the state of the first one; 
			// otherwise save the ostringstream state
			if(!output_tee.stream_ptr_list.empty())
				os_ptr = output_tee.stream_ptr_list.front();
			else if(!output_tee.view_ptr_list.empty())
				os_ptr = &output_tee.view_line_buffer;
			else
				return;
			
			old_flags = os_ptr->flags();
			old_precision = os_ptr->precision();
		}
	
	~Output_tee_format_saver()
		{
			// apply the saved formatting to all streams
			for (std::list<std::ostream *>::iterator it = output_tee.stream_ptr_list.begin(); it != output_tee.stream_ptr_list.end(); it++) {
				std::ostream * os_ptr = *it;
				os_ptr->flags(old_flags);
				os_ptr->precision(old_precision);
				}
			// apply the saved formatting to the ostringstream if views are present
			if(!output_tee.view_ptr_list.empty()) {
				output_tee.view_line_buffer.flags(old_flags);
				output_tee.view_line_buffer.precision(old_precision);
				}
		}
private:
	Output_tee& output_tee;
	std::ios::fmtflags old_flags;
	std::streamsize old_precision;
};


	
#endif
