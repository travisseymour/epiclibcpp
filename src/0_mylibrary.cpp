#include <pybind11/pybind11.h>

//------- this section is temp 4lines vvv, trying to see if this matters
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
// ------

#include <pybind11/iostream.h>

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
namespace py = pybind11;

using namespace pybind11::literals;  // to bring in the `_a` literal

#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <list>
#include <stdexcept>
#include <sstream>

#include "auditory_encoder_base.h"
#include "auditory_perceptual_processor.h"
#include "clause.h"
#include "coordinator.h"
#include "device_base.h"
#include "device_exception.h"
#include "device_processor.h"
#include "human_processor.h"
#include "human_subprocessor.h"
#include "model.h"
#include "point.h"
#include "output_tee.h"
#include "output_tee_globals.h"
#include "pps_globals.h"
#include "production_system.h"
#include "speech_word.h"
#include "visual_encoder_base.h"
#include "visual_perceptual_processor.h"
#include "view_base.h"
#include "numeric_utilities.h"
#include "epic_exception.h"
#include "random_utilities.h"
#include "symbol_utilities.h"
#include "syllable_counter.h"
#include "standard_symbols.h"
#include "epic_standard_symbols.h"
#include "standard_utility_symbols.h"
#include "statistics.h"
#include "rms_statistics.h"
#include "processor.h"
#include "production_system.h"
#include "event_types.h"

using namespace std;
using namespace Geometry_Utilities;
using std::string;
using std::to_string;

PYBIND11_MAKE_OPAQUE(std::vector<Parameter_specification>)
PYBIND11_MAKE_OPAQUE(std::vector<Symbol>)

extern Output_tee Normal_out;
extern Output_tee Trace_out;
extern Output_tee PPS_out;
extern Output_tee Debug_out;
extern Output_tee Device_out;
extern Output_tee Exception_out;
extern Output_tee Stats_out;

class ProductionSystem: public Parsimonious_Production_System::Production_System {};


// ********************************************************
// other
// ********************************************************

//void describe_parameters(const Model& model, Output_tee& ot)
//{
//    model.get_human_ptr()->describe_parameters(ot);
//}

string describe_parameters_u(const Model& model)
{
//    cout << "\nEPICLIB: Describe Parameters:\n";
//    if (!Normal_out.is_present(cout)) {
//        Normal_out.add_stream(cout);
//    }
//    model.get_human_ptr()->describe_parameters(Normal_out);
//    cout << "EPICLIB: done.\n";
//    if (!Normal_out.is_present(cout)) {
//        Normal_out.remove_stream(cout);
//    }

    Output_tee temp_out;
    ostringstream temp_str_stream;
    temp_out.add_stream(temp_str_stream);
    model.get_human_ptr()->describe_parameters(temp_out);
    temp_out.remove_stream(temp_str_stream);
    return temp_str_stream.str();

}

void set_visual_encoder_ptr(const Model& model, Visual_encoder_base * encoder)
{
    model.get_human_ptr()->set_visual_encoder_ptr(encoder);
}

void set_auditory_encoder_ptr(const Model& model, Auditory_encoder_base * encoder)
{
    model.get_human_ptr()->set_aditory_encoder_ptr(encoder);
}


// ********************************************************
// Trampoline Classes
// ********************************************************

class PyDevice_base : public Device_base {
public:
    using Device_base::Device_base;

    // Trampoline (need one for each virtual function)
    void set_parameter_string(const std::string& _str) override {
        PYBIND11_OVERRIDE(void, /* Return type */
        Device_base, /* Parent class */
        set_parameter_string, /* Name of function in C++ (must match Python name) */
        _str /* parameter(s) */
        );
    }

    std::string get_parameter_string() const override {
        PYBIND11_OVERRIDE(std::string, Device_base, get_parameter_string, /* no args */ );
    }

    void initialize() override {
        PYBIND11_OVERRIDE( void, Device_base,  initialize, /* no args */ );
    }

    std::string processor_info() const override {
        PYBIND11_OVERRIDE( std::string, Device_base, processor_info, /* no args */ );
    }

    void display() const override {
        PYBIND11_OVERRIDE(void, Device_base, display, /* no args */ );
    }

    void handle_Start_event() override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Start_event, /* no args */ );
    }

    void handle_Stop_event() override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Stop_event, /* no args */ );
    }

    void handle_Report_event(long duration) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Report_event, duration);
    }

    void handle_Delay_event(const Symbol& _type, const Symbol& datum,
                             const Symbol& object_name, const Symbol& property_name,
                             const Symbol& property_value) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Delay_event, _type, datum,
                               object_name, property_name, property_value);
    }

    void handle_Keystroke_event(const Symbol& key_name) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Keystroke_event, key_name);
    }

    void handle_Type_In_event(const Symbol& type_in_string) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Type_In_event, type_in_string);
    }

    void handle_Hold_event(const Symbol& button_name) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Hold_event, button_name);
    }

    void handle_Release_event(const Symbol& button_name) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Release_event, button_name);
    }

    void handle_Click_event(const Symbol& button_name) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Click_event, button_name);
    }

    void handle_Double_Click_event(const Symbol& button_name) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Double_Click_event, button_name);
    }

    void handle_Point_event(const Symbol& target_name) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Point_event, target_name);
    }

    void handle_Ply_event(const Symbol& cursor_name, const Symbol& target_name,
                           GU::Point new_location, GU::Polar_vector movement_vector) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Ply_event, cursor_name,
                               target_name, new_location, movement_vector);
    }

    void handle_Vocal_event(const Symbol& vocal_input) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Vocal_event, vocal_input);
    }

    void handle_Vocal_event(const Symbol& vocal_input, long duration) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Vocal_event, vocal_input, duration);
    }

    void handle_VisualFocusChange_event(const Symbol& object_name) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_VisualFocusChange_event, object_name);
    }

    void handle_Eyemovement_Start_event(const Symbol& target_name, GU::Point new_location) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Eyemovement_Start_event, target_name, new_location);
    }

    void handle_Eyemovement_End_event(const Symbol& target_name, GU::Point new_location) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_Eyemovement_End_event, target_name, new_location);
    }

    void handle_HLGet_event(const Symbol_list_t& props, const Symbol_list_t& values, const Symbol& tag) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_HLGet_event, props, values, tag);
    }

    void handle_HLPut_event(const Symbol_list_t& props, const Symbol_list_t& values) override {
        PYBIND11_OVERRIDE(void, Device_base, handle_HLPut_event, props, values);
    }

};


struct PyView_base : public View_base {

    using View_base::View_base;  // inherit ctors (default or otherwise)

    void clear() override {
        PYBIND11_OVERRIDE(void, View_base, clear, /* no args */);
    }

    void notify_eye_movement(GU::Point p) override {
        PYBIND11_OVERRIDE(void, View_base, notify_eye_movement, p);
    }

    void notify_object_appear(const Symbol& s, GU::Point p, GU::Size sz) override {
        PYBIND11_OVERRIDE(void, View_base, notify_object_appear, s, p, sz);
    }

    void notify_object_disappear(const Symbol& s) override {
        PYBIND11_OVERRIDE(void, View_base, notify_object_disappear, s);
    }

    void notify_object_reappear(const Symbol& s) override {
        PYBIND11_OVERRIDE(void, View_base, notify_object_reappear, s);
    }

    void notify_erase_object(const Symbol& s) override {
        PYBIND11_OVERRIDE(void, View_base, notify_erase_object, s);
    }

    void notify_visual_location_changed(const Symbol& symbol, GU::Point point) override {
        PYBIND11_OVERRIDE(void,View_base,notify_visual_location_changed,symbol,point);
    }

    void notify_visual_size_changed(const Symbol& symbol, GU::Size size) override {
        PYBIND11_OVERRIDE(void,View_base,notify_visual_size_changed,symbol,size);
    }

    void notify_visual_property_changed(const Symbol& symbol1, const Symbol& symbol2, const Symbol& symbol3) override {
        PYBIND11_OVERRIDE(void,View_base,notify_visual_property_changed,symbol1,symbol2,symbol3);
    }

    void notify_auditory_stream_appear(const Symbol& symbol, double d1, double d2, GU::Point point) override {
        PYBIND11_OVERRIDE(void,View_base,notify_auditory_stream_appear,symbol,d1,d2,point);
    }

    void notify_auditory_stream_disappear(const Symbol& symbol) override {
        PYBIND11_OVERRIDE(void,View_base,notify_auditory_stream_disappear,symbol);
    }

    void notify_auditory_stream_location_changed(const Symbol& symbol, GU::Point point) override {
        PYBIND11_OVERRIDE(void,View_base,notify_auditory_stream_location_changed,symbol,point);
    }

    void notify_auditory_stream_pitch_changed(const Symbol& symbol, double d) override {
        PYBIND11_OVERRIDE(void,View_base,notify_auditory_stream_pitch_changed,symbol,d);
    }

    void notify_auditory_stream_loudness_changed(const Symbol& symbol, double d) override {
        PYBIND11_OVERRIDE(void,View_base,notify_auditory_stream_loudness_changed,symbol,d);
    }

    void notify_auditory_stream_size_changed(const Symbol& symbol, GU::Size size) override {
        PYBIND11_OVERRIDE(void,View_base,notify_auditory_stream_size_changed,symbol,size );
    }

    void notify_auditory_stream_property_changed(const Symbol& symbol1, const Symbol& symbol2, const Symbol& symbol3) override {
        PYBIND11_OVERRIDE(void,View_base,notify_auditory_stream_property_changed,symbol1,symbol2,symbol3 );
    }

    void notify_auditory_sound_start(const Symbol& sym1, const Symbol& sym2, long num1, GU::Point point, const Symbol& sym3, double num2) override {
        PYBIND11_OVERRIDE(void, View_base, notify_auditory_sound_start, sym1, sym2, num1, point, sym3, num2);
    }

    void notify_auditory_speech_start(const Speech_word& word) override {
        PYBIND11_OVERRIDE(void, View_base, notify_auditory_speech_start, word);
    }

    void notify_auditory_sound_stop(const Symbol& sym) override {
        PYBIND11_OVERRIDE(void, View_base, notify_auditory_sound_stop, sym);
    }

    void notify_erase_sound(const Symbol& sym) override {
        PYBIND11_OVERRIDE(void, View_base, notify_erase_sound, sym);
    }

    void notify_auditory_sound_property_changed(const Symbol& sym1, const Symbol& sym2, const Symbol& sym3) override {
        PYBIND11_OVERRIDE(void, View_base, notify_auditory_sound_property_changed, sym1, sym2, sym3);
    }

    void notify_append_text(const std::string& str) override {
        PYBIND11_OVERRIDE(void, View_base, notify_append_text, str);
    }

    void notify_time(long num) override {
        PYBIND11_OVERRIDE(void, View_base, notify_time, num);
    }

};

class PyVisual_encoder_base : public Visual_encoder_base {

    /* Inherit the constructors */
    using Visual_encoder_base::Visual_encoder_base;

    /* Trampoline (need one for each virtual function) */
    void initialize() override {
        PYBIND11_OVERRIDE(void, Visual_encoder_base, initialize, /* no args */);
    }

    bool set_object_property(const Symbol& object_name, const Symbol& property_name, const Symbol& property_value, long encoding_time) override {
        PYBIND11_OVERRIDE(bool, Visual_encoder_base, set_object_property, object_name, property_name, property_value, encoding_time);
    }

    bool handle_Delay_event(const Symbol& object_name, const Symbol& property_name, const Symbol& property_value) override {
        PYBIND11_OVERRIDE(bool, Visual_encoder_base, handle_Delay_event, object_name, property_name, property_value);
    }
};

class PyAuditory_encoder_base : public Auditory_encoder_base {
public:
    /* Inherit the constructors */
    using Auditory_encoder_base::Auditory_encoder_base;

    /* Trampoline (need one for each virtual function) */
    void initialize() override {
        PYBIND11_OVERRIDE(void, Auditory_encoder_base, initialize, /* no args */);
    }

    bool set_object_property(const Symbol& object_name, const Symbol& property_name, const Symbol& property_value, long encoding_time) override {
        PYBIND11_OVERRIDE(bool, Auditory_encoder_base, set_object_property, object_name, property_name, property_value, encoding_time);
    }

    bool handle_Delay_event(const Symbol& object_name, const Symbol& property_name, const Symbol& property_value) override {
        PYBIND11_OVERRIDE(bool, Auditory_encoder_base, handle_Delay_event, object_name, property_name, property_value);
    }

    GU::Point recode_location(GU::Point original_location) override {
        PYBIND11_OVERRIDE(GU::Point, Auditory_encoder_base, recode_location, original_location);
    }
};

// *********************************************************
// Pybind11 Interface
// *********************************************************


PYBIND11_MODULE(epiclib, m) {
    m.doc() = "EPICLib bindings for Python - Travis Seymour, PhD";
    // py::bind_vector<std::vector<Parameter_specification>>(m, "Parameter_specification_list_t");
    // py::bind_vector<std::vector<Symbol>>(m, "Symbol_list_t");

    // ----- other ---------
    m.def("describe_parameters_u", &describe_parameters_u,
          py::doc("access to describe_parameters() without referencing a Human pointer"));

    m.def("set_visual_encoder_ptr", &set_visual_encoder_ptr,
        py::doc("access to set_visual_encoder_ptr() without referencing a Human pointer"));

    m.def("set_auditory_encoder_ptr", &set_auditory_encoder_ptr,
        py::doc("access to set_auditory_encoder_ptr() without referencing a Human pointer"));
    //---------------------


//    py::add_ostream_redirect(m, "ostream_redirect");

    py::class_<Device_exception>(m, "Device_exception")
        .def(py::init())
        .def(py::init<const std::string&>(), py::arg("msg_"))
        .def(py::init<const Device_base *, const std::string&>(),py::arg("proc_ptr"), py::arg("msg_"))
        ;

    py::class_<Epic_exception>(m, "Epic_exception")
        .def(py::init())
        .def(py::init<const std::string&>(), py::arg("msg_"))
        .def(py::init<Processor *, const std::string&>(),py::arg("proc_ptr"), py::arg("msg_"))
        ;

    m.def("raise_device_exception", [](const std::string &str) {
        throw Device_exception(str);
    });

    m.def("raise_epic_exception", [](const std::string &str) {
        throw Epic_exception(str);
    });

    // Submodule to mirror your header name for convenience:
    auto m_globals = m.def_submodule("output_tee_globals", "Global Output_tee instances");

    // Output_tee class (Python-facing helpers only; you can expose more if you want)
    auto pyOutputTee = py::class_<Output_tee>(m, "Output_tee")
        .def("add_view",
            static_cast<void (Output_tee::*)(View_base*)>(&Output_tee::add_view),
            py::arg("view"),
            py::keep_alive<1,2>()  // keep Python view alive while attached to this tee
            )
        .def("remove_view",
            static_cast<void (Output_tee::*)(View_base*)>(&Output_tee::remove_view),
            py::arg("view")
            )
        .def("add_py_stream", &Output_tee::add_py_stream, py::arg("stream"),
             "Attach any Python object with write/flush/close.")
        .def("remove_py_stream", &Output_tee::remove_py_stream, py::arg("stream"))
        .def("clear_py_streams", &Output_tee::clear_py_streams)
        .def("py_write", &Output_tee::py_write, py::arg("text"),
             "Write a string (identical to the C++ << path).")
        .def("py_flush", &Output_tee::py_flush)
        .def("py_close", &Output_tee::py_close)
        // Nice ergonomic sugar so you can do: Normal_out("hello")
        .def("__call__", &Output_tee::py_write, py::arg("text"));

    // Bind the global tee instances by reference
    m_globals.attr("Normal_out") = py::cast(&Normal_out, py::return_value_policy::reference);
    m_globals.attr("Trace_out")  = py::cast(&Trace_out,  py::return_value_policy::reference);
    m_globals.attr("Debug_out")  = py::cast(&Debug_out,  py::return_value_policy::reference);
    m_globals.attr("Device_out")  = py::cast(&Device_out,  py::return_value_policy::reference);
    m_globals.attr("Exception_out")  = py::cast(&Exception_out,  py::return_value_policy::reference);
    m_globals.attr("Stats_out")  = py::cast(&Stats_out,  py::return_value_policy::reference);

    auto pps_globals = m.def_submodule("pps_globals", "Global PPS Output_tee instance");
    pps_globals.attr("PPS_out")  = py::cast(&PPS_out,  py::return_value_policy::reference);


    py::class_<Clause>(m, "Clause")
        .def(py::init())
        .def(py::init<Symbol&, Symbol&, Symbol&, Symbol&>(), py::arg("tag_name"), py::arg("object_name"), py::arg("property_name"), py::arg("property_value"))
        .def(py::init<Symbol_list_t&>(), py::arg("in_term_list"))
        .def(py::init<std::string&>(), py::arg("in_string"))
        .def(py::init<Clause&>(), py::arg("in_clause"));

    py::class_<Symbol>(m, "Symbol")
        .def(py::init<const char*, bool>(), py::arg("c")="Nil", py::arg("check_for_number")=true)
        .def(py::init<const std::string&, bool>(), py::arg("s"), py::arg("check_for_number")=true)
        .def(py::init<double>())
        .def(py::init<int>())
        .def(py::init<long>())
        .def(py::init<double, double>())
        .def(py::init<GU::Point>())
        .def(py::init<const std::vector<GU::Point>&>())
        .def(py::init<const char*, const std::vector<GU::Point>&>(), py::arg("c"), py::arg("v"))
        .def(py::init<const Symbol&>())
        .def("has_string_value", &Symbol::has_string_value)
        .def("has_numeric_value", &Symbol::has_numeric_value)
        .def("has_single_numeric_value", &Symbol::has_single_numeric_value)
        .def("has_point_numeric_value", &Symbol::has_Point_numeric_value)
        .def("has_pair_numeric_value", &Symbol::has_pair_numeric_value)
        .def("has_multiple_numeric_value", &Symbol::has_multiple_numeric_value)
        .def("c_str", &Symbol::c_str)
        .def("size", &Symbol::size)
        .def("length", &Symbol::length)
        .def("str", &Symbol::str)
        .def("get_numeric_value", &Symbol::get_numeric_value)
        .def("get_point", &Symbol::get_Point)
        .def("get_x", &Symbol::get_x)
        .def("get_y", &Symbol::get_y)
        .def("get_point_vector", &Symbol::get_Point_vector)

        .def("__eq__", [](const Symbol& lhs, const char * c){return lhs == c;})
        .def("__ne__", [](const Symbol& lhs, const char * c){return lhs != c;})
        .def("__lt__", [](const Symbol& lhs, const char * c){return lhs < c;})
        .def("__le__", [](const Symbol& lhs, const char * c){return lhs <= c;})
        .def("__gt__", [](const Symbol& lhs, const char * c){return lhs > c;})
        .def("__ge__", [](const Symbol& lhs, const char * c){return lhs >= c;})

        .def("__eq__", [](const Symbol& lhs, const Symbol& rhs){return lhs == rhs;})
        .def("__ne__", [](const Symbol& lhs, const Symbol& rhs){return lhs != rhs;})
        .def("__lt__", [](const Symbol& lhs, const Symbol& rhs){return lhs < rhs;})
        .def("__le__", [](const Symbol& lhs, const Symbol& rhs){return lhs <= rhs;})
        .def("__gt__", [](const Symbol& lhs, const Symbol& rhs){return lhs > rhs;})
        .def("__ge__", [](const Symbol& lhs, const Symbol& rhs){return lhs >= rhs;})

        .def("__eq__", [](const Symbol& lhs, double d){return lhs == d;})
        .def("__ne__", [](const Symbol& lhs, double d){return lhs != d;})
        .def("__lt__", [](const Symbol& lhs, double d){return lhs < d;})
        .def("__le__", [](const Symbol& lhs, double d){return lhs <= d;})
        .def("__gt__", [](const Symbol& lhs, double d){return lhs > d;})
        .def("__ge__", [](const Symbol& lhs, double d){return lhs >= d;})

        .def("swap", &Symbol::swap)
        .def("__repr__", [](const Symbol& sw) {
              ostringstream os;
              if (sw.has_string_value()) {
                os << sw.c_str();
              } else if (sw.has_single_numeric_value())  {
                os << sw.get_numeric_value();
              } else if (sw.has_Point_numeric_value()) {
                Point p = sw.get_Point();
                os << "Point(" << p.x << ", " << p.y << ")";
              } else if (sw.has_multiple_numeric_value()) {
                os << sw.get_Point_vector();
              } else {
                os << "<empty>";
              }

              return os.str();
        })
        ;

    py::class_<Speech_word>(m, "Speech_word")
        .def(py::init<>())
        .def_readwrite("name", &Speech_word::name)
        .def_readwrite("stream_name", &Speech_word::stream_name)
        .def_readwrite("time_stamp", &Speech_word::time_stamp)
        .def_readwrite("location", &Speech_word::location)
        .def_readwrite("pitch", &Speech_word::pitch)
        .def_readwrite("loudness", &Speech_word::loudness)
        .def_readwrite("duration", &Speech_word::duration)
        .def_readwrite("level_left", &Speech_word::level_left)  // = 0
        .def_readwrite("level_right", &Speech_word::level_right)  // = 0
        .def_readwrite("content", &Speech_word::content)
        .def_readwrite("speaker_gender", &Speech_word::speaker_gender)
        .def_readwrite("speaker_id", &Speech_word::speaker_id)
        .def_readwrite("utterance_id", &Speech_word::utterance_id)
        .def("__repr__", [](const Speech_word& sw) {
              ostringstream os;
              os << "Speech_word(name=" << sw.name.c_str() << ", location=Point(" << sw.location.x << "," << sw.location.y << "), content=" << sw.content.c_str() << ")";
              return os.str();
        })
        ;

    py::class_<Device_base, PyDevice_base, std::unique_ptr<Device_base>>(m, "Device_base")
        .def(py::init<const std::string&, Output_tee&>(), py::arg("Id"), py::arg("ot").none(true))
        .def("set_parameter_string", &Device_base::set_parameter_string, py::arg("_str"))
        .def("get_parameter_string", &Device_base::get_parameter_string)
        .def("get_name", &Device_base::get_name)
        .def("initialize", &Device_base::initialize)
        .def("processor_info", &Device_base::processor_info)
        .def("get_device_proc_ptr", &Device_base::get_device_proc_ptr)
        .def("handle_Start_event", &Device_base::handle_Start_event)
        .def("handle_Stop_event", &Device_base::handle_Stop_event)
        .def("handle_Report_event", &Device_base::handle_Report_event, py::arg("duration"))
        .def("handle_Delay_event", &Device_base::handle_Delay_event, py::arg("_type"), py::arg("datum"), py::arg("object_name"), py::arg("property_name"), py::arg("property_value"))
        .def("handle_Keystroke_event", &Device_base::handle_Keystroke_event, py::arg("key_name"))
        .def("handle_Type_In_event", &Device_base::handle_Type_In_event, py::arg("type_in_string"))
        .def("handle_Hold_event", &Device_base::handle_Hold_event, py::arg("button_name"))
        .def("handle_Release_event", &Device_base::handle_Release_event, py::arg("button_name"))
        .def("handle_Click_event", &Device_base::handle_Click_event, py::arg("button_name"))
        .def("handle_Double_Click_event", &Device_base::handle_Double_Click_event, py::arg("button_name"))
        .def("handle_Point_event", &Device_base::handle_Point_event, py::arg("target_name"))
        .def("handle_Ply_event", &Device_base::handle_Ply_event, py::arg("cursor_name"), py::arg("target_name"), py::arg("new_location"), py::arg("movement_vector"))
        .def("handle_Vocal_event", static_cast<void (Device_base::*)(const Symbol&)>(&Device_base::handle_Vocal_event), py::arg("vocal_input"))
        .def("handle_Vocal_event", static_cast<void (Device_base::*)(const Symbol&, long)>(&Device_base::handle_Vocal_event), py::arg("vocal_input"), py::arg("duration"))
        .def("handle_VisualFocusChange_event", &Device_base::handle_VisualFocusChange_event, py::arg("object_name"))
        .def("handle_Eyemovement_Start_event", &Device_base::handle_Eyemovement_Start_event, py::arg("target_name"), py::arg("new_location"))
        .def("handle_Eyemovement_End_event", &Device_base::handle_Eyemovement_End_event, py::arg("target_name"), py::arg("new_location"))
        .def("handle_HLGet_event", &Device_base::handle_HLGet_event, py::arg("props"), py::arg("values"), py::arg("tag"))
        .def("handle_HLPut_event", &Device_base::handle_HLPut_event, py::arg("props"), py::arg("values"))
        .def("get_time", &Device_base::get_time)
        .def("get_trace", &Device_base::get_trace)
        .def("set_trace", &Device_base::set_trace, py::arg("trace_"))
        .def("make_visual_object_appear", static_cast<void (Device_base::*)(const Symbol&)>(&Device_base::make_visual_object_appear), py::arg("object_name"))
        .def("make_visual_object_appear", static_cast<void (Device_base::*)(const Symbol&, GU::Point, GU::Size)>(&Device_base::make_visual_object_appear), py::arg("object_name"), py::arg("location"), py::arg("size"))
        .def("set_visual_object_location", &Device_base::set_visual_object_location, py::arg("object_name"), py::arg("new_location"))
        .def("set_visual_object_size", &Device_base::set_visual_object_size, py::arg("object_name"), py::arg("new_size"))
        .def("set_visual_object_property", &Device_base::set_visual_object_property, py::arg("object_name"), py::arg("property_name"), py::arg("property_value"))
        .def("make_visual_object_disappear", &Device_base::make_visual_object_disappear, py::arg("object_name"))
        .def("set_auditory_stream_location", &Device_base::set_auditory_stream_location, py::arg("name"), py::arg("location"))
        .def("set_auditory_stream_size", &Device_base::set_auditory_stream_size, py::arg("name"), py::arg("size"))
        .def("set_auditory_stream_property", &Device_base::set_auditory_stream_property, py::arg("name"), py::arg("propname"), py::arg("propvalue"))
        .def("make_auditory_event", &Device_base::make_auditory_event, py::arg("message"))
        .def("make_auditory_sound_event", &Device_base::make_auditory_sound_event, py::arg("name"), py::arg("stream"), py::arg("location"), py::arg("timbre"), py::arg("loudness"), py::arg("duration"), py::arg("intrinsic_duration"))
        .def("make_auditory_sound_start", &Device_base::make_auditory_sound_start, py::arg("name"), py::arg("stream"), py::arg("location"), py::arg("timbre"), py::arg("loudness"), py::arg("intrinsic_duration"))
        .def("make_auditory_sound_stop", &Device_base::make_auditory_sound_stop, py::arg("name"))
        .def("set_auditory_sound_property", &Device_base::set_auditory_sound_property, py::arg("name"), py::arg("propname"), py::arg("propvalue"))
        .def("make_auditory_speech_event", &Device_base::make_auditory_speech_event, py::arg("word"))
        .def("make_high_level_input_appear", &Device_base::make_high_level_input_appear, py::arg("object_name"), py::arg("props"), py::arg("values"), py::arg("tag"))
        .def("make_high_level_input_disappear", &Device_base::make_high_level_input_disappear, py::arg("object_name"))
//        ??? Whats the diff between these 2 approaches??
//        .def("schedule_delay_event", static_cast<void (Device_base::*)(long)>(&Device_base::schedule_delay_event), py::arg("delay"))
//        .def("schedule_delay_event", static_cast<void (Device_base::*)(long, const Symbol&, const Symbol&)>(&Device_base::schedule_delay_event), py::arg("delay"), py::arg("delay_type"), py::arg("delay_datum"))
//        .def("schedule_delay_event", static_cast<void (Device_base::*)(long, const Symbol&, const Symbol&, const Symbol&, const Symbol&)>(&Device_base::schedule_delay_event), py::arg("delay"), py::arg("delay_type"), py::arg("object_name"), py::arg("property_name"), py::arg("property_value"))
        .def("schedule_delay_event", (void (Device_base::*)(long)) &Device_base::schedule_delay_event, py::arg("delay"))
        .def("schedule_delay_event", (void (Device_base::*)(long, const Symbol&, const Symbol&)) &Device_base::schedule_delay_event, py::arg("delay"), py::arg("delay_type"), py::arg("delay_datum"))
        .def("schedule_delay_event", (void (Device_base::*)(long, const Symbol&, const Symbol&, const Symbol&, const Symbol&)) &Device_base::schedule_delay_event, py::arg("delay"), py::arg("delay_type"), py::arg("object_name"), py::arg("property_name"), py::arg("property_value"))

        .def("make_report", &Device_base::make_report, py::arg("time"), py::arg("duration"))
        .def("set_human_parameter", &Device_base::set_human_parameter, py::arg("proc_name"), py::arg("param_name"), py::arg("spec"))
        .def("get_human_prs_filename", &Device_base::get_human_prs_filename)
        .def("stop_simulation", &Device_base::stop_simulation)
        .def("write", &Device_base::write, py::arg("msg"))

//        .def("__del__", [](MyClass& self) {
//            // free resources in __del__ method
//        })

        ;

//    py::class_<Parsimonious_Production_System::Production_System>(m, "Production_System")
//        .def(py::init<>())
//        .def("get_cycle_counter", &Parsimonious_Production_System::Production_System::get_cycle_counter)
//        .def("get_output_compiler_messages", &Parsimonious_Production_System::Production_System::get_output_compiler_messages)
//        .def("set_output_compiler_messages", &Parsimonious_Production_System::Production_System::set_output_compiler_messages)
//        .def("get_output_compiler_details", &Parsimonious_Production_System::Production_System::get_output_compiler_details)
//        .def("set_output_compiler_details", &Parsimonious_Production_System::Production_System::set_output_compiler_details)
//        .def("get_output_run_messages", &Parsimonious_Production_System::Production_System::get_output_run_messages)
//        .def("set_output_run_messages", &Parsimonious_Production_System::Production_System::set_output_run_messages)
//        .def("get_output_run_details", &Parsimonious_Production_System::Production_System::get_output_run_details)
//        .def("set_output_run_details", &Parsimonious_Production_System::Production_System::set_output_run_details)
//        .def("get_output_run_memory_contents", &Parsimonious_Production_System::Production_System::get_output_run_memory_contents)
//        .def("set_output_run_memory_contents", &Parsimonious_Production_System::Production_System::set_output_run_memory_contents)
//        .def("clear", &Parsimonious_Production_System::Production_System::clear)
//        .def("compile_file", &Parsimonious_Production_System::Production_System::compile_file)
//        .def("display_static_contents", &Parsimonious_Production_System::Production_System::display_static_contents)
//        .def("display_dynamic_contents", &Parsimonious_Production_System::Production_System::display_dynamic_contents)
//        .def("reset", &Parsimonious_Production_System::Production_System::reset)
//        .def("initialize", &Parsimonious_Production_System::Production_System::initialize)
//        .def("run", &Parsimonious_Production_System::Production_System::run)
//        .def("run_cycle", &Parsimonious_Production_System::Production_System::run_cycle)
//        .def("update_add_clause", &Parsimonious_Production_System::Production_System::update_add_clause)
//        .def("update_delete_clause", &Parsimonious_Production_System::Production_System::update_delete_clause)
//        .def("add_to_add_clause_list", &Parsimonious_Production_System::Production_System::add_to_add_clause_list)
//        .def("add_to_delete_clause_list", &Parsimonious_Production_System::Production_System::add_to_delete_clause_list)
//        .def("add_initial_clause", &Parsimonious_Production_System::Production_System::add_initial_clause)
//        .def("get_root_node_ptr", &Parsimonious_Production_System::Production_System::get_root_node_ptr)
//        .def("get_named_location", &Parsimonious_Production_System::Production_System::get_named_location)
//        .def("get_parameter_specifications", &Parsimonious_Production_System::Production_System::get_parameter_specifications)
//        .def("add_pattern_node", &Parsimonious_Production_System::Production_System::add_pattern_node)
//        .def("add_rule_node", &Parsimonious_Production_System::Production_System::add_rule_node)
//        .def("add_to_fired_rules", &Parsimonious_Production_System::Production_System::add_to_fired_rules)
//        .def("remove_from_fired_rules", &Parsimonious_Production_System::Production_System::remove_from_fired_rules)
//        .def("memory_updated", &Parsimonious_Production_System::Production_System::memory_updated)
//        .def("display_memory_contents", &Parsimonious_Production_System::Production_System::display_memory_contents)
//        .def("get_rule_names", &Parsimonious_Production_System::Production_System::get_rule_names)
//        .def("is_rule_name_valid", &Parsimonious_Production_System::Production_System::is_rule_name_valid)
//        .def("get_break_enabled", &Parsimonious_Production_System::Production_System::get_break_enabled)
//        .def("set_break_enabled", &Parsimonious_Production_System::Production_System::set_break_enabled)
//        .def("set_rule_break_state", &Parsimonious_Production_System::Production_System::set_rule_break_state)
//        .def("get_rule_break_state", &Parsimonious_Production_System::Production_System::get_rule_break_state)
//        .def("get_break_rule_names", &Parsimonious_Production_System::Production_System::get_break_rule_names)
//        .def("get_fault_enabled", &Parsimonious_Production_System::Production_System::get_fault_enabled)
//        .def("set_fault_enabled", &Parsimonious_Production_System::Production_System::set_fault_enabled)
//        .def("set_rule_fault_state", &Parsimonious_Production_System::Production_System::set_rule_fault_state)
//        .def("get_rule_fault_state", &Parsimonious_Production_System::Production_System::get_rule_fault_state)
//        .def("get_break_on_this_cycle", &Parsimonious_Production_System::Production_System::get_break_on_this_cycle)
//        .def("get_fault_rule_names", &Parsimonious_Production_System::Production_System::get_fault_rule_names)
//        ;

    py::class_<Model, std::unique_ptr<Model>>(m, "Model")
        .def(py::init<Device_base *>(), py::arg("device_ptr_"))
        .def("get_human_ptr", &Model::get_human_ptr)
        .def("get_device_ptr", &Model::get_device_ptr)
        .def("set_device_ptr", &Model::set_device_ptr, py::arg("device_ptr_"))
        .def("get_device_processor_ptr", &Model::get_device_processor_ptr)
        .def("connect_device_to_human", &Model::connect_device_to_human, py::arg("dev_base_ptr"))
        .def("get_visual_encoder_ptr", &Model::get_visual_encoder_ptr)
        .def("set_visual_encoder_ptr", &Model::set_visual_encoder_ptr, py::arg("visual_encoder_ptr"))
        .def("get_auditory_encoder_ptr", &Model::get_auditory_encoder_ptr)
        .def("set_auditory_encoder_ptr", &Model::set_auditory_encoder_ptr, py::arg("auditory_encoder_ptr"))
        .def("compile", &Model::compile)
        .def("initialize", &Model::initialize)
        .def("run_time", &Model::run_time, py::arg("duration"))
        .def("stop", &Model::stop)
        .def("get_time", &Model::get_time)
        .def("display_memory_contents", &Model::display_memory_contents)
        .def("get_prs_filename", &Model::get_prs_filename)
        .def("set_prs_filename", &Model::set_prs_filename, py::arg("fn"))
        .def("get_compiled", &Model::get_compiled)
        .def("get_initialized", &Model::get_initialized)
        .def("get_running", &Model::get_running)
        .def("add_visual_physical_view", &Model::add_visual_physical_view, py::arg("view_ptr"))
        .def("add_visual_sensory_view", &Model::add_visual_sensory_view, py::arg("view_ptr"))
        .def("add_visual_perceptual_view", &Model::add_visual_perceptual_view, py::arg("view_ptr"))
        .def("remove_visual_physical_view", &Model::remove_visual_physical_view, py::arg("view_ptr"))
        .def("remove_visual_sensory_view", &Model::remove_visual_sensory_view, py::arg("view_ptr"))
        .def("remove_visual_perceptual_view", &Model::remove_visual_perceptual_view, py::arg("view_ptr"))
        .def("add_auditory_physical_view", &Model::add_auditory_physical_view, py::arg("view_ptr"))
        .def("add_auditory_sensory_view", &Model::add_auditory_sensory_view, py::arg("view_ptr"))
        .def("add_auditory_perceptual_view", &Model::add_auditory_perceptual_view, py::arg("view_ptr"))
        .def("remove_auditory_physical_view", &Model::remove_auditory_physical_view, py::arg("view_ptr"))
        .def("remove_auditory_sensory_view", &Model::remove_auditory_sensory_view, py::arg("view_ptr"))
        .def("remove_auditory_perceptual_view", &Model::remove_auditory_perceptual_view, py::arg("view_ptr"))
        .def("add_view", &Model::add_view, py::arg("view_ptr"))
        .def("remove_view", &Model::remove_view, py::arg("view_ptr"))
        .def("remove_all_views", &Model::remove_all_views)
        .def("clear_all_views", &Model::clear_all_views)
        .def("get_trace_all", &Model::get_trace_all)
        .def("set_trace_all", &Model::set_trace_all, py::arg("trace_"))
        .def("get_trace_visual", &Model::get_trace_visual)
        .def("set_trace_visual", &Model::set_trace_visual, py::arg("trace_"))
        .def("get_trace_auditory", &Model::get_trace_auditory)
        .def("set_trace_auditory", &Model::set_trace_auditory, py::arg("trace_"))
        .def("get_trace_cognitive", &Model::get_trace_cognitive)
        .def("set_trace_cognitive", &Model::set_trace_cognitive, py::arg("trace_"))
        .def("get_trace_ocular", &Model::get_trace_ocular)
        .def("set_trace_ocular", &Model::set_trace_ocular, py::arg("trace_"))
        .def("get_trace_manual", &Model::get_trace_manual)
        .def("set_trace_manual", &Model::set_trace_manual, py::arg("trace_"))
        .def("get_trace_vocal", &Model::get_trace_vocal)
        .def("set_trace_vocal", &Model::set_trace_vocal, py::arg("trace_"))
        .def("get_trace_temporal", &Model::get_trace_temporal)
        .def("set_trace_temporal", &Model::set_trace_temporal, py::arg("trace_"))
        .def("get_trace_device", &Model::get_trace_device)
        .def("set_trace_device", &Model::set_trace_device, py::arg("trace_"))
        .def("get_output_compiler_messages", &Model::get_output_compiler_messages)
        .def("set_output_compiler_messages", &Model::set_output_compiler_messages, py::arg("flag"))
        .def("get_output_compiler_details", &Model::get_output_compiler_details)
        .def("set_output_compiler_details", &Model::set_output_compiler_details, py::arg("flag"))
        .def("get_output_run_messages", &Model::get_output_run_messages)
        .def("set_output_run_messages", &Model::set_output_run_messages, py::arg("flag"))
        .def("get_output_run_details", &Model::get_output_run_details)
        .def("set_output_run_details", &Model::set_output_run_details, py::arg("flag"))
        .def("get_output_run_memory_contents", &Model::get_output_run_memory_contents)
        .def("set_output_run_memory_contents", &Model::set_output_run_memory_contents, py::arg("flag"))
        .def("get_rule_names", &Model::get_rule_names)
        .def("is_rule_name_valid", &Model::is_rule_name_valid, py::arg("rule_name"))
        .def("get_break_enabled", &Model::get_break_enabled)
        .def("set_break_enabled", &Model::set_break_enabled, py::arg("state"))
        .def("set_rule_break_state", &Model::set_rule_break_state, py::arg("rule_name"), py::arg("state"))
        .def("get_rule_break_state", &Model::get_rule_break_state, py::arg("rule_name"))
        .def("get_break_rule_names", &Model::get_break_rule_names)
        .def("get_fault_enabled", &Model::get_fault_enabled)
        .def("set_fault_enabled", &Model::set_fault_enabled, py::arg("state"))
        .def("set_rule_fault_state", &Model::set_rule_fault_state, py::arg("rule_name"), py::arg("state"))
        .def("get_rule_fault_state", &Model::get_rule_fault_state, py::arg("rule_name"))
        .def("get_fault_rule_names", &Model::get_fault_rule_names)
        .def("set_parameters", &Model::set_parameters, py::arg("parameter_specs"))
        .def("set_parameter", &Model::set_parameter, py::arg("proc_name"), py::arg("param_name"), py::arg("spec"))
        .def("set_random_number_generator_seed", &Model::set_random_number_generator_seed, py::arg("seed"))
        .def("get_random_number_generator_seed", &Model::get_random_number_generator_seed)
        .def("set_device_parameter_string", &Model::set_device_parameter_string, py::arg("str_"))
        .def("get_device_parameter_string", &Model::get_device_parameter_string)
        .def("interconnect_device_and_human", &Model::interconnect_device_and_human)
//        .def("get_production_system_ptr", &Model::get_production_system_ptr)
        ;



    // NOTE: Can't copy (e.g. my_instance = Coordinator.get_instance())
    //       For now, using Coordinator directly, but if that don't work, may need to
    //       create an object above that serves as an interface and expose that instead.
    //       or this: https://stackoverflow.com/questions/41814652/how-to-wrap-a-singleton-class-using-pybind11
    py::class_<Coordinator, std::unique_ptr<Coordinator>>(m, "Coordinator")
        .def_static("get_instance", &Coordinator::get_instance, py::return_value_policy::reference)
        .def_static("get_time", &Coordinator::get_time)
        .def("initialize", &Coordinator::initialize)
        .def("run_until_done", &Coordinator::run_until_done)
        .def("run_for", &Coordinator::run_for, py::arg("run_duration"))
        .def("pause", &Coordinator::pause)
        .def("stop", &Coordinator::stop)
        .def("shutdown_simulation", &Coordinator::shutdown_simulation)
        .def("is_not_ready", &Coordinator::is_not_ready)
        .def("is_runnable", &Coordinator::is_runnable)
        .def("is_timed_out", &Coordinator::is_timed_out)
        .def("is_paused", &Coordinator::is_paused)
        .def("is_finished", &Coordinator::is_finished)
        .def("add_processor", &Coordinator::add_processor)
        .def("remove_processor", &Coordinator::remove_processor)
        .def("schedule_event", &Coordinator::schedule_event, py::arg("p"))
        .def("call_with_event", &Coordinator::call_with_event, py::arg("msg"))
        ;

    py::module_ gu = m.def_submodule("geometric_utilities");
    gu.def("to_radians", &Geometry_Utilities::to_radians, py::arg("theta_d"));
    gu.def("degrees_subtended", &Geometry_Utilities::degrees_subtended, py::arg("size_measure"), py::arg("distance_measure"));
    gu.def("degrees_subtended_per_unit", &Geometry_Utilities::degrees_subtended_per_unit, py::arg("units_per_measure"), py::arg("distance_measure"));
    gu.def("units_per_degree_subtended", &Geometry_Utilities::units_per_degree_subtended, py::arg("units_per_measure"), py::arg("distance_measure"));
    gu.def("is_point_inside_rectangle", &Geometry_Utilities::is_point_inside_rectangle, py::arg("p"), py::arg("rect_loc"), py::arg("rect_size"));
    gu.def("clip_line_to_rectangle", &Geometry_Utilities::clip_line_to_rectangle, py::arg("line"), py::arg("rect_loc"), py::arg("rect_size"));
    gu.def("compute_center_intersecting_line", &Geometry_Utilities::compute_center_intersecting_line, py::arg("start_to_center"), py::arg("rect_size"), py::arg("clipped_line"));
    gu.def("closest_distance", &Geometry_Utilities::closest_distance, py::arg("p"), py::arg("rect_center"), py::arg("rect_size"));
    gu.def("cartesian_distance", &Geometry_Utilities::cartesian_distance, py::arg("p1"), py::arg("p2"));

    py::class_<Point>(gu, "Point")
        .def(py::init<double, double>(), py::arg("in_x")=0.0, py::arg("in_y")=0.0)
        .def_property("x",&Point::get_x, &Point::set_x)
        .def_property("y",&Point::get_y, &Point::set_y)

        .def("__add__", [](const Point& p, const Cartesian_vector& cv){return p + cv;})
        .def("__add__", [](const Point& p, const Polar_vector& pv){return p + pv;})
        .def("__sub__", [](const Point& p1, const Point& p2){return p1 - p2;})

        .def("__repr__", [](const Point& pt) {
              ostringstream os;
              os << "Point(" << pt.x << ", " << pt.y << ")";
              return os.str();
        });

    py::class_<Geometry_Utilities::Size>(gu, "Size")
        .def(py::init<double, double>(), py::arg("in_h")=0.0, py::arg("in_v")=0.0)
        .def_readwrite("h", &Geometry_Utilities::Size::h)
        .def_readwrite("v", &Geometry_Utilities::Size::v)
        .def("__repr__", [](const Geometry_Utilities::Size& sz) {
              std::ostringstream os;
              os << "Size(" << sz.h << ", " << sz.v << ")";
              return os.str();
        });

    py::class_<Geometry_Utilities::Cartesian_vector>(gu, "Cartesian_vector")
        .def(py::init<double, double>(), py::arg("delta_x")=0.0, py::arg("delta_y")=0.0)
        .def(py::init<Point, Point>(), py::arg("p1"), py::arg("p2"))
        .def(py::init<Geometry_Utilities::Polar_vector>(), py::arg("pv"))
        .def_readwrite("delta_x", &Geometry_Utilities::Cartesian_vector::delta_x)
        .def_readwrite("delta_y", &Geometry_Utilities::Cartesian_vector::delta_y)

        .def("__add__", [](const Cartesian_vector& cv, const Point p){return cv + p;})
        .def("__add__", [](const Cartesian_vector& cv1, const Cartesian_vector& cv2){return cv1 + cv2;})
        .def("__sub__", [](const Cartesian_vector& cv1, const Cartesian_vector& cv2){return cv1 - cv2;})
        .def("__div__", [](const Cartesian_vector& cv, double d){return cv / d;})
        .def("__mul__", [](const Cartesian_vector& cv, double d){return cv * d;})

        .def("__repr__", [](const Geometry_Utilities::Cartesian_vector& cv) {
              std::ostringstream os;
              os << "CartesianVector(" << cv.delta_x << ", " << cv.delta_y << ")";
              return os.str();
        });
        
    py::class_<Geometry_Utilities::Polar_vector>(gu, "Polar_vector")
        .def(py::init<double, double>(), py::arg("in_r")=0.0, py::arg("in_theta")=0.0)
        .def(py::init<Point, Point>(), py::arg("p1"), py::arg("p2"))
        .def(py::init<Geometry_Utilities::Cartesian_vector>(), py::arg("cv"))
        .def_readwrite("r", &Geometry_Utilities::Polar_vector::r)
        .def_readwrite("theta", &Geometry_Utilities::Polar_vector::theta)

        .def("__add__", [](const Polar_vector& pv, const Point p){return pv + p;})
        .def("__div__", [](const Polar_vector& pv, double d){return pv / d;})
        .def("__mul__", [](const Polar_vector& pv, double d){return pv * d;})

        .def("__repr__", [](const Geometry_Utilities::Polar_vector& pv) {
              std::ostringstream os;
              os << "PolarVector(" << pv.r << ", " << pv.theta << ")";
              return os.str();
        });

    py::class_<Geometry_Utilities::Line_segment>(gu, "Line_segment")
        .def(py::init())
        .def(py::init<Point, Point>(), py::arg("in_p1"), py::arg("in_p2"))
        .def("get_p1", &Geometry_Utilities::Line_segment::get_p1)
        .def("get_p2", &Geometry_Utilities::Line_segment::get_p2)
        .def("get_center", &Geometry_Utilities::Line_segment::get_center)
        .def("get_size", &Geometry_Utilities::Line_segment::get_size)
        .def("get_dx", &Geometry_Utilities::Line_segment::get_dx)
        .def("get_dy", &Geometry_Utilities::Line_segment::get_dy)
        .def("get_A", &Geometry_Utilities::Line_segment::get_A)
        .def("get_B", &Geometry_Utilities::Line_segment::get_B)
        .def("get_C", &Geometry_Utilities::Line_segment::get_C)
        .def("get_length", &Geometry_Utilities::Line_segment::get_length)
        .def("is_horizontal", &Geometry_Utilities::Line_segment::is_horizontal)
        .def("is_vertical", &Geometry_Utilities::Line_segment::is_vertical)
        .def("is_on_infinite_line", &Geometry_Utilities::Line_segment::is_on_infinite_line, py::arg("p"))
        .def("closest_point_on_infinite_line", &Geometry_Utilities::Line_segment::closest_point_on_infinite_line, py::arg("p"))
        .def("distance_from_infinite_line", &Geometry_Utilities::Line_segment::distance_from_infinite_line, py::arg("p"))
        .def("parameter", &Geometry_Utilities::Line_segment::parameter, py::arg("p"))
        .def("parameter_given_x", &Geometry_Utilities::Line_segment::parameter_given_x, py::arg("x"))
        .def("x_given_parameter", &Geometry_Utilities::Line_segment::x_given_parameter, py::arg("t"))
        .def("parameter_given_y", &Geometry_Utilities::Line_segment::parameter_given_y, py::arg("y"))
        .def("y_given_parameter", &Geometry_Utilities::Line_segment::y_given_parameter, py::arg("t"))
        .def("point_on_line", &Geometry_Utilities::Line_segment::point_on_line, py::arg("t"))
        .def("closest_point_on_segment", &Geometry_Utilities::Line_segment::closest_point_on_segment, py::arg("p"))
        .def("distance_from_segment", &Geometry_Utilities::Line_segment::distance_from_segment, py::arg("p"))
        .def("__repr__", [](const Geometry_Utilities::Line_segment& lseg) {
              std::ostringstream os;
              os << "LineSegment(" << lseg.get_p1() << ", " << lseg.get_p2() << ")";
              return os.str();
        });

    py::class_<Geometry_Utilities::Polygon>(gu, "Polygon")
        .def(py::init())
        .def(py::init<std::vector<Point>&>(), py::arg("in_vertices"))
        .def("add_vertex", &Geometry_Utilities::Polygon::add_vertex, py::arg("p"))
        .def("clear", &Geometry_Utilities::Polygon::clear)
        .def("get_center", &Geometry_Utilities::Polygon::get_center)
        .def("get_size", &Geometry_Utilities::Polygon::get_size)
        .def("distance_inside", &Geometry_Utilities::Polygon::distance_inside, py::arg("p"))
        .def("get_vertices", &Geometry_Utilities::Polygon::get_vertices)
        .def("__repr__", [](const Geometry_Utilities::Polygon& pg) {
              std::ostringstream os;
              os << "Polygon(";
              for (Point p: pg.get_vertices())
                  os << "Point(" << p.x << ", " << p.y << ")" << ", ";
              os << ")";
              return os.str();
        })
        ;

    // Expose View_base and the trampoline so Python can subclass it if needed
    py::class_<View_base, PyView_base>(m, "View_base")
        .def(py::init())
        .def("clear", &View_base::clear)
        .def("notify_eye_movement", &View_base::notify_eye_movement, py::arg("GU::Point"))
        .def("notify_object_appear", &View_base::notify_object_appear, py::arg("Symbol&"), py::arg("GU::Point"), py::arg("GU::Size"))
        .def("notify_object_disappear", &View_base::notify_object_disappear, py::arg("Symbol&"))
        .def("notify_object_reappear", &View_base::notify_object_reappear, py::arg("Symbol&"))
        .def("notify_erase_object", &View_base::notify_erase_object, py::arg("Symbol&"))
        .def("notify_visual_location_changed", &View_base::notify_visual_location_changed, py::arg("Symbol&"), py::arg("GU::Point"))
        .def("notify_visual_size_changed", &View_base::notify_visual_size_changed, py::arg("Symbol&"), py::arg("GU::Size"))
        .def("notify_visual_property_changed", &View_base::notify_visual_property_changed, py::arg("Symbol&"), py::arg("Symbol&"), py::arg("Symbol&"))
        .def("notify_auditory_stream_appear", &View_base::notify_auditory_stream_appear, py::arg("Symbol&"), py::arg("double"), py::arg("double"), py::arg("GU::Point"))
        .def("notify_auditory_stream_disappear", &View_base::notify_auditory_stream_disappear, py::arg("Symbol&"))
        .def("notify_auditory_stream_location_changed", &View_base::notify_auditory_stream_location_changed, py::arg("Symbol&"), py::arg("GU::Point"))
        .def("notify_auditory_stream_pitch_changed", &View_base::notify_auditory_stream_pitch_changed, py::arg("Symbol&"), py::arg("double"))
        .def("notify_auditory_stream_loudness_changed", &View_base::notify_auditory_stream_loudness_changed, py::arg("Symbol&"), py::arg("double"))
        .def("notify_auditory_stream_size_changed", &View_base::notify_auditory_stream_size_changed, py::arg("Symbol&"), py::arg("GU::Size"))
        .def("notify_auditory_stream_property_changed", &View_base::notify_auditory_stream_property_changed, py::arg("Symbol&"), py::arg("Symbol&"), py::arg("Symbol&"))
        .def("notify_auditory_sound_start", &View_base::notify_auditory_sound_start, py::arg("Symbol&"), py::arg("Symbol&"), py::arg("long"), py::arg("GU::Point"), py::arg("Symbol&"), py::arg("double"))
        .def("notify_auditory_speech_start", &View_base::notify_auditory_speech_start, py::arg("Speech_word&"))
        .def("notify_auditory_sound_stop", &View_base::notify_auditory_sound_stop, py::arg("Symbol&"))
        .def("notify_erase_sound", &View_base::notify_erase_sound, py::arg("Symbol&"))
        .def("notify_auditory_sound_property_changed", &View_base::notify_auditory_sound_property_changed, py::arg("Symbol&"), py::arg("Symbol&"), py::arg("Symbol&"))
        .def("notify_append_text", &View_base::notify_append_text, py::arg("std::string&"))
        .def("notify_time", &View_base::notify_time, py::arg("long"))
        .def("attach_to", &View_base::attach_to, py::arg("processor_ptr"))
        .def("detach_from", &View_base::detach_from, py::arg("processor_ptr"))
        .def("detach_from_all", &View_base::detach_from_all)
        ;

    py::class_<Visual_encoder_base, PyVisual_encoder_base, std::unique_ptr<Visual_encoder_base>>(m, "Visual_encoder_base")
        .def(py::init<const std::string&>(), py::arg("id"))
        .def("get_name", &Visual_encoder_base::get_name)
        .def("initialize", &Visual_encoder_base::initialize)
        .def("connect", &Visual_encoder_base::connect, py::arg("perceptual_proc_ptr_"))
        .def("get_perceptual_proc_ptr", &Visual_encoder_base::get_perceptual_proc_ptr)
        .def("set_object_property", &Visual_encoder_base::set_object_property, py::arg("object_name"), py::arg("property_name"), py::arg("property_value"), py::arg("encoding_time"))
        .def("handle_Delay_event", &Visual_encoder_base::handle_Delay_event, py::arg("object_name"), py::arg("property_name"), py::arg("property_value"))

        .def("schedule_change_property_event", &Visual_encoder_base::schedule_change_property_event, py::arg("recoding_time"), py::arg("object_name"), py::arg("property_name"), py::arg("property_value") )

        .def("__repr__",
            [](const Visual_encoder_base& v) {
                return "Visual_encoder_base '" + v.get_name() + "'>";
            }
        )
        ;

    py::class_<Auditory_encoder_base, PyAuditory_encoder_base, std::unique_ptr<Auditory_encoder_base>>(m, "Auditory_encoder_base")
        .def(py::init<const std::string&>(), py::arg("id"))
        .def("get_name", &Auditory_encoder_base::get_name)
        .def("initialize", &Auditory_encoder_base::initialize)
        .def("get_perceptual_proc_ptr", &Auditory_encoder_base::get_perceptual_proc_ptr)
        .def("connect", &Auditory_encoder_base::connect, py::arg("perceptual_proc_ptr_"))
        .def("set_object_property", &Auditory_encoder_base::set_object_property, py::arg("object_name"), py::arg("property_name"), py::arg("property_value"), py::arg("encoding_time"))
        .def("handle_Delay_event", &Auditory_encoder_base::handle_Delay_event, py::arg("object_name"), py::arg("property_name"), py::arg("property_value"))

        .def("schedule_change_property_event", &Auditory_encoder_base::schedule_change_property_event, py::arg("recoding_time"), py::arg("object_name"), py::arg("property_name"), py::arg("property_value") )


        .def("__repr__",
            [](const Auditory_encoder_base& v) {
                return "<Auditory_encoder_base '" + v.get_name() + "'>";
            }
        )
        ;

    py::module_ nu = m.def_submodule("numeric_utilities");
    nu.doc() = "EPICLib Numeric utilities";
    nu.def("round_to_integer", &round_to_integer, "Return a double rounded to the nearest integer value.");
    nu.def("int_to_string", &int_to_string, "Convert an integer to a string.");
    nu.def("time_convert", (long (*)(long, long, long)) &time_convert, "Convert hours, minutes, and seconds to milliseconds (note long integer returned).");
    nu.def("time_convert", [](long, int&, int&, int&, int&) {
        throw std::runtime_error("There is no pybind interface for this updater version of time_convert.");
    });
    nu.def("time_convert", [](long, int&, int&, double&) {
        throw std::runtime_error("There is no pybind interface for this updater version of time_convert.");
    });
    nu.def("logb2", &logb2, "Compute the base 2 log of the supplied value.");
    nu.def("pitch_to_semitones", &pitch_to_semitones, "Convert pitch to semitones.");

    py::module_ ru = m.def_submodule("random_utilities");
    ru.doc() = "EPICLib module for random number generation";
    ru.def("set_random_number_generator_seed", &set_random_number_generator_seed, "Set the seed for the random number generator");
    ru.def("random_int", &random_int, "Generate a random integer in the range [0, range-1]");
    ru.def("biased_coin_flip", &biased_coin_flip, "Flip a biased coin and return true with probability p");
    ru.def("unit_uniform_random_variable", &unit_uniform_random_variable, "Generate a unit uniform random variable");
    ru.def("uniform_random_variable", &uniform_random_variable, "Generate a uniform random variable with given mean and deviation");
    ru.def("unit_normal_random_variable", &unit_normal_random_variable, "Generate a unit normal random variable");
    ru.def("normal_random_variable", &normal_random_variable, "Generate a normal random variable with given mean and sd");
    ru.def("log_normal_random_variable", &log_normal_random_variable, "Generate a log-normal random variable with given mean and sd");
    ru.def("uniform_detection_function", &uniform_detection_function, "Generate a boolean based on a uniform detection function");
    ru.def("gaussian_detection_function", &gaussian_detection_function, "Generate a boolean based on a Gaussian detection function");
    ru.def("lapsed_gaussian_detection_function", &lapsed_gaussian_detection_function, "Generate a boolean based on a lapsed Gaussian detection function");
    ru.def("based_gaussian_detection_function", &based_gaussian_detection_function, "Generate a boolean based on a based Gaussian detection function");
    ru.def("capped_gaussian_detection_function", &capped_gaussian_detection_function, "Generate a boolean based on a capped Gaussian detection function");
    ru.def("exponential_detection_function", &exponential_detection_function, "Generate a boolean based on an exponential detection function");
    ru.def("based_exponential_detection_function", &based_exponential_detection_function, "Generate a boolean based on a based exponential detection function");
    ru.def("get_bivariate_normal_cdf", &get_bivariate_normal_cdf, "Get the cumulative probability from the bivariate normal distribution for two given z-scores");
    py::class_<Discrete_distribution>(ru, "Discrete_distribution")
        .def(py::init<std::vector<double>>(), "Constructor for Discrete_distribution class, takes in a vector of probabilities")
        .def("get_random_value", &Discrete_distribution::get_random_value, "Get a random value based on the given probabilities");

    py::module_ su = m.def_submodule("symbol_utilities");
    su.doc() = "EPICLib module for Symbol manipulation";
    su.def("get_nth_Symbol", (Symbol (*)(Symbol_list_t&, long)) &get_nth_Symbol, "Return the nth Symbol in the list");
//    su.def("print_Symbol_list", (Symbol (*)(std::vector<Symbol> *)) &print_Symbol_list, "Print a Symbol list");
//    su.def("print_Symbol_list", (Symbol (*)(std::vector<Symbol> *, std::ostream &)) &print_Symbol_list, "Print a Symbol list to an output stream");
    su.def("cstr_to_Symbol_list", &cstr_to_Symbol_list, "Convert a C-string to a Symbol list");
    su.def("int_to_Symbol", &int_to_Symbol, "Convert an integer to a Symbol");
    su.def("concatenate_to_Symbol", (Symbol (*)(const char *, long)) &concatenate_to_Symbol, "Concatenate to a Symbol with a string and an integer");
    su.def("concatenate_to_Symbol", (Symbol (*)(const Symbol&, long)) &concatenate_to_Symbol, "Concatenate to a Symbol with a Symbol and an integer");
    su.def("concatenate_to_Symbol", (Symbol (*)(const char *, const char *, long)) &concatenate_to_Symbol, "Concatenate to a Symbol with two strings and an integer");
    su.def("concatenate_to_Symbol", (Symbol (*)(const Symbol&, const Symbol&, long)) &concatenate_to_Symbol, "Concatenate to a Symbol with two Symbols and an integer");
    su.def("concatenate_to_Symbol", (Symbol (*)(const char*, const Symbol&, long)) &concatenate_to_Symbol, "Concatenate to a Symbol with a string, a Symbol, and an integer");
    su.def("concatenate_to_Symbol", (Symbol (*)(const Symbol&, const char*, long)) &concatenate_to_Symbol, "Concatenate to a Symbol with a Symbol, a string, and an integer");
    su.def("concatenate_to_Symbol", (Symbol (*)(const char *, const char *, const char *, long)) &concatenate_to_Symbol, "Concatenate to a Symbol with three strings and an integer");
    su.def("concatenate_to_Symbol", (Symbol (*)(const char*, const Symbol&, const char*, long)) &concatenate_to_Symbol, "Concatenate to a Symbol with a string, a Symbol, another string, and an integer");

    py::module_ sc = m.def_submodule("syllable_counter");
    sc.doc() = "EPICLib module to count syllables";
    sc.def("count_total_syllables", (long (*)(const std::string&)) &count_total_syllables);

    py::module_ ss = m.def_submodule("standard_symbols");
    ss.doc() = "EPICLib module containing standard symbols";
    ss.attr("Cursor_name_c") = py::cast(&Cursor_name_c);
    ss.attr("Tracking_cursor_name_c") = py::cast(&Tracking_cursor_name_c);
    ss.attr("Mouse_name_c") = py::cast(&Mouse_name_c);
    ss.attr("Mouse_Left_Button_c") = py::cast(&Mouse_Left_Button_c);
    ss.attr("Mouse_Middle_Button_c") = py::cast(&Mouse_Middle_Button_c);
    ss.attr("Mouse_Right_Button_c") = py::cast(&Mouse_Right_Button_c);
    ss.attr("Keyboard_name_c") = py::cast(&Keyboard_name_c);
    ss.attr("Status_c") = py::cast(&Status_c);
    ss.attr("Visual_c") = py::cast(&Visual_c);
    ss.attr("Visual_status_c") = py::cast(&Visual_status_c);
    ss.attr("New_c") = py::cast(&New_c);
    ss.attr("Visible_c") = py::cast(&Visible_c);
    ss.attr("Targetable_c") = py::cast(&Targetable_c);
    ss.attr("Disappearing_c") = py::cast(&Disappearing_c);
    ss.attr("Reappearing_c") = py::cast(&Reappearing_c);
    ss.attr("Disappeared_object_c") = py::cast(&Disappeared_object_c);
    ss.attr("Change_c") = py::cast(&Change_c);
    ss.attr("Color_changed_c") = py::cast(&Color_changed_c);
    ss.attr("BgColor_changed_c") = py::cast(&BgColor_changed_c);
    ss.attr("FgColor_changed_c") = py::cast(&FgColor_changed_c);
    ss.attr("Color_c") = py::cast(&Color_c);
    ss.attr("Color_Vague_c") = py::cast(&Color_Vague_c);
    ss.attr("BgColor_c") = py::cast(&BgColor_c);
    ss.attr("FgColor_c") = py::cast(&FgColor_c);
    ss.attr("Shape_c") = py::cast(&Shape_c);
    ss.attr("Size_c") = py::cast(&Size_c);
    ss.attr("Encoded_size_c") = py::cast(&Encoded_size_c);
    ss.attr("Label_c") = py::cast(&Label_c);
    ss.attr("Text_c") = py::cast(&Text_c);
    ss.attr("Position_c") = py::cast(&Position_c);
    ss.attr("Vposition_c") = py::cast(&Vposition_c);
    ss.attr("Hposition_c") = py::cast(&Hposition_c);
    ss.attr("Location_c") = py::cast(&Location_c);
    ss.attr("Orientation_c") = py::cast(&Orientation_c);
    ss.attr("Distance_c") = py::cast(&Distance_c);
    ss.attr("Depth_c") = py::cast(&Depth_c);
    ss.attr("Leader_c") = py::cast(&Leader_c);
    ss.attr("Horizontal_c") = py::cast(&Horizontal_c);
    ss.attr("Vertical_c") = py::cast(&Vertical_c);
    ss.attr("Top_c") = py::cast(&Top_c);
    ss.attr("Bottom_c") = py::cast(&Bottom_c);
    ss.attr("Type_c") = py::cast(&Type_c);
    ss.attr("Screen_c") = py::cast(&Screen_c);
    ss.attr("Window_c") = py::cast(&Window_c);
    ss.attr("Dialog_c") = py::cast(&Dialog_c);
    ss.attr("Menu_c") = py::cast(&Menu_c);
    ss.attr("Menu_item_c") = py::cast(&Menu_item_c);
    ss.attr("Field_c") = py::cast(&Field_c);
    ss.attr("Aqua_c") = py::cast(&Aqua_c);
    ss.attr("Black_c") = py::cast(&Black_c);
    ss.attr("Blue_c") = py::cast(&Blue_c);
    ss.attr("Brown_c") = py::cast(&Brown_c);
    ss.attr("Chartreuse_c") = py::cast(&Chartreuse_c);
    ss.attr("Cyan_c") = py::cast(&Cyan_c);
    ss.attr("DarkBlue_c") = py::cast(&DarkBlue_c);
    ss.attr("DarkGray_c") = py::cast(&DarkGray_c);
    ss.attr("DarkGreen_c") = py::cast(&DarkGreen_c);
    ss.attr("DarkRed_c") = py::cast(&DarkRed_c);
    ss.attr("DarkViolet_c") = py::cast(&DarkViolet_c);
    ss.attr("Gainsboro_c") = py::cast(&Gainsboro_c);
    ss.attr("Green_c") = py::cast(&Green_c);
    ss.attr("Gray_c") = py::cast(&Gray_c);
    ss.attr("Fuchsia_c") = py::cast(&Fuchsia_c);
    ss.attr("Gold_c") = py::cast(&Gold_c);
    ss.attr("GoldenRod_c") = py::cast(&GoldenRod_c);
    ss.attr("LightBlue_c") = py::cast(&LightBlue_c);
    ss.attr("LightGray_c") = py::cast(&LightGray_c);
    ss.attr("Magenta_c") = py::cast(&Magenta_c);
    ss.attr("Maroon_c") = py::cast(&Maroon_c);
    ss.attr("Navy_c") = py::cast(&Navy_c);
    ss.attr("Olive_c") = py::cast(&Olive_c);
    ss.attr("Pink_c") = py::cast(&Pink_c);
    ss.attr("Purple_c") = py::cast(&Purple_c);
    ss.attr("Red_c") = py::cast(&Red_c);
    ss.attr("RoyalBlue_c") = py::cast(&RoyalBlue_c);
    ss.attr("SlateGray_c") = py::cast(&SlateGray_c);
    ss.attr("Teal_c") = py::cast(&Teal_c);
    ss.attr("Turquoise_c") = py::cast(&Turquoise_c);
    ss.attr("Violet_c") = py::cast(&Violet_c);
    ss.attr("White_c") = py::cast(&White_c);
    ss.attr("Yellow_c") = py::cast(&Yellow_c);
    ss.attr("Vague_c") = py::cast(&Vague_c);
    ss.attr("Circle_c") = py::cast(&Circle_c );
    ss.attr("Empty_Circle_c") = py::cast(&Empty_Circle_c );
    ss.attr("Filled_Circle_c") = py::cast(&Filled_Circle_c );
    ss.attr("Top_Semicircle_c") = py::cast(&Top_Semicircle_c );
    ss.attr("Empty_Top_Semicircle_c") = py::cast(&Empty_Top_Semicircle_c );
    ss.attr("Filled_Top_Semicircle_c") = py::cast(&Filled_Top_Semicircle_c );
    ss.attr("Rectangle_c") = py::cast(&Rectangle_c );
    ss.attr("Empty_Rectangle_c") = py::cast(&Empty_Rectangle_c );
    ss.attr("Filled_Rectangle_c") = py::cast(&Filled_Rectangle_c );
    ss.attr("Square_c") = py::cast(&Square_c );
    ss.attr("Empty_Square_c") = py::cast(&Empty_Square_c );
    ss.attr("Filled_Square_c") = py::cast(&Filled_Square_c );
    ss.attr("Button_c") = py::cast(&Button_c );
    ss.attr("Empty_Button_c") = py::cast(&Empty_Button_c );
    ss.attr("Filled_Button_c") = py::cast(&Filled_Button_c );
    ss.attr("Triangle_c") = py::cast(&Triangle_c);
    ss.attr("Empty_Triangle_c") = py::cast(&Empty_Triangle_c);
    ss.attr("Filled_Triangle_c") = py::cast(&Filled_Triangle_c);
    ss.attr("Diamond_c") = py::cast(&Diamond_c);
    ss.attr("Empty_Diamond_c") = py::cast(&Empty_Diamond_c);
    ss.attr("Filled_Diamond_c") = py::cast(&Filled_Diamond_c);
    ss.attr("House_c") = py::cast(&House_c);
    ss.attr("Empty_House_c") = py::cast(&Empty_House_c);
    ss.attr("Filled_House_c") = py::cast(&Filled_House_c);
    ss.attr("Inv_House_c") = py::cast(&Inv_House_c);
    ss.attr("Inv_Empty_House_c") = py::cast(&Inv_Empty_House_c);
    ss.attr("Inv_Filled_House_c") = py::cast(&Inv_Filled_House_c);
    ss.attr("Cross_c") = py::cast(&Cross_c);
    ss.attr("Empty_Cross_c") = py::cast(&Empty_Cross_c);
    ss.attr("Filled_Cross_c") = py::cast(&Filled_Cross_c);
    ss.attr("Bar_c") = py::cast(&Bar_c);
    ss.attr("Empty_Bar_c") = py::cast(&Empty_Bar_c);
    ss.attr("Filled_Bar_c") = py::cast(&Filled_Bar_c);
    ss.attr("Clover_c") = py::cast(&Clover_c);
    ss.attr("Empty_Clover_c") = py::cast(&Empty_Clover_c);
    ss.attr("Filled_Clover_c") = py::cast(&Filled_Clover_c);
    ss.attr("Clover3_c") = py::cast(&Clover3_c);
    ss.attr("Empty_Clover3_c") = py::cast(&Empty_Clover3_c);
    ss.attr("Filled_Clover3_c") = py::cast(&Filled_Clover3_c);
    ss.attr("Inv_Clover3_c") = py::cast(&Inv_Clover3_c);
    ss.attr("Inv_Empty_Clover3_c") = py::cast(&Inv_Empty_Clover3_c);
    ss.attr("Inv_Filled_Clover3_c") = py::cast(&Inv_Filled_Clover3_c);
    ss.attr("Heart_c") = py::cast(&Heart_c);
    ss.attr("Empty_Heart_c") = py::cast(&Empty_Heart_c);
    ss.attr("Filled_Heart_c") = py::cast(&Filled_Heart_c);
    ss.attr("Hill_c") = py::cast(&Hill_c);
    ss.attr("Empty_Hill_c") = py::cast(&Empty_Hill_c);
    ss.attr("Filled_Hill_c") = py::cast(&Filled_Hill_c);
    ss.attr("Inv_Hill_c") = py::cast(&Inv_Hill_c);
    ss.attr("Inv_Empty_Hill_c") = py::cast(&Inv_Empty_Hill_c);
    ss.attr("Inv_Filled_Hill_c") = py::cast(&Inv_Filled_Hill_c);
    ss.attr("Cross_Hairs_c") = py::cast(&Cross_Hairs_c);
    ss.attr("Cursor_Arrow_c") = py::cast(&Cursor_Arrow_c);
    ss.attr("Up_Arrow_c") = py::cast(&Up_Arrow_c);
    ss.attr("Down_Arrow_c") = py::cast(&Down_Arrow_c);
    ss.attr("Left_Arrow_c") = py::cast(&Left_Arrow_c);
    ss.attr("Right_Arrow_c") = py::cast(&Right_Arrow_c);
    ss.attr("Line_c") = py::cast(&Line_c);
    ss.attr("Polygon_c") = py::cast(&Polygon_c);
    ss.attr("Empty_Polygon_c") = py::cast(&Empty_Polygon_c);
    ss.attr("Filled_Polygon_c") = py::cast(&Filled_Polygon_c);
    ss.attr("North_Leader_c") = py::cast(&North_Leader_c);
    ss.attr("North_c") = py::cast(&North_c);
    ss.attr("South_Leader_c") = py::cast(&South_Leader_c);
    ss.attr("South_c") = py::cast(&South_c);
    ss.attr("East_Leader_c") = py::cast(&East_Leader_c);
    ss.attr("East_c") = py::cast(&East_c);
    ss.attr("West_Leader_c") = py::cast(&West_Leader_c);
    ss.attr("West_c") = py::cast(&West_c);
    ss.attr("T000_c") = py::cast(&T000_c);
    ss.attr("T090_c") = py::cast(&T090_c);
    ss.attr("T135_c") = py::cast(&T135_c);
    ss.attr("T180_c") = py::cast(&T180_c);
    ss.attr("T270_c") = py::cast(&T270_c);
    ss.attr("L000_c") = py::cast(&L000_c);
    ss.attr("L090_c") = py::cast(&L090_c);
    ss.attr("L135_c") = py::cast(&L135_c);
    ss.attr("L180_c") = py::cast(&L180_c);
    ss.attr("L270_c") = py::cast(&L270_c);
    ss.attr("Block_X_c") = py::cast(&Block_X_c);
    ss.attr("Block_Y_c") = py::cast(&Block_Y_c);
    ss.attr("Left_of_c") = py::cast(&Left_of_c);
    ss.attr("Right_of_c") = py::cast(&Right_of_c);
    ss.attr("Above_c") = py::cast(&Above_c);
    ss.attr("Below_c") = py::cast(&Below_c);
    ss.attr("Inside_c") = py::cast(&Inside_c);
    ss.attr("In_center_of_c") = py::cast(&In_center_of_c);
    ss.attr("Outside_c") = py::cast(&Outside_c);
    ss.attr("In_row_c") = py::cast(&In_row_c);
    ss.attr("In_col_c") = py::cast(&In_col_c);
    ss.attr("Placed_on_c") = py::cast(&Placed_on_c);
    ss.attr("Pointing_to_c") = py::cast(&Pointing_to_c);
    ss.attr("Auditory_c") = py::cast(&Auditory_c);
    ss.attr("Auditory_status_c") = py::cast(&Auditory_status_c);
    ss.attr("Stream_c") = py::cast(&Stream_c);
    ss.attr("Azimuth_c") = py::cast(&Azimuth_c);
    ss.attr("Audible_c") = py::cast(&Audible_c);
    ss.attr("Fading_c") = py::cast(&Fading_c);
    ss.attr("Pitch_c") = py::cast(&Pitch_c);
    ss.attr("Loudness_c") = py::cast(&Loudness_c);
    ss.attr("Timbre_c") = py::cast(&Timbre_c);
    ss.attr("Present_c") = py::cast(&Present_c);
    ss.attr("Next_c") = py::cast(&Next_c);
    ss.attr("Time_stamp_c") = py::cast(&Time_stamp_c);
    ss.attr("External_c") = py::cast(&External_c);
    ss.attr("Default_physical_stream_c") = py::cast(&Default_physical_stream_c);
    ss.attr("Default_psychological_stream_c") = py::cast(&Default_psychological_stream_c);
    ss.attr("Speech_c") = py::cast(&Speech_c);
    ss.attr("Content_c") = py::cast(&Content_c);
    ss.attr("Gender_c") = py::cast(&Gender_c);
    ss.attr("Male_c") = py::cast(&Male_c);
    ss.attr("Female_c") = py::cast(&Female_c);
    ss.attr("Speaker_c") = py::cast(&Speaker_c);
    ss.attr("Signal_c") = py::cast(&Signal_c);
    ss.attr("Start_c") = py::cast(&Start_c);
    ss.attr("Stop_c") = py::cast(&Stop_c);
    ss.attr("Halt_c") = py::cast(&Halt_c);
    ss.attr("End_c") = py::cast(&End_c);
    ss.attr("Increment_c") = py::cast(&Increment_c);

    py::module_ sus = m.def_submodule("standard_utility_symbols");
    sus.doc() = "EPICLib module containing standard utility symbols";
    sus.attr("Default_c") = py::cast(&Default_c);
    sus.attr("Absent_c") = py::cast(&Absent_c);
    sus.attr("Unknown_c") = py::cast(&Unknown_c);
    sus.attr("None_c") = py::cast(&None_c);
    sus.attr("Nil_c") = py::cast(&Nil_c);
    sus.attr("Empty_string_c") = py::cast(&Empty_string_c);

    py::module_ ess = m.def_submodule("epic_standard_symbols");
    ess.doc() = "EPICLib module containing EPIC standard symbols";
    ess.attr("Detection_c") = py::cast(&Detection_c);
    ess.attr("Onset_c") = py::cast(&Onset_c);
    ess.attr("Offset_c") = py::cast(&Offset_c);
    ess.attr("Start_time_c") = py::cast(&Start_time_c);
    ess.attr("End_time_c") = py::cast(&End_time_c);
    ess.attr("Eccentricity_c") = py::cast(&Eccentricity_c);
    ess.attr("Eccentricity_zone_c") = py::cast(&Eccentricity_zone_c);
    ess.attr("Fovea_c") = py::cast(&Fovea_c);
    ess.attr("Periphery_c") = py::cast(&Periphery_c);
    ess.attr("Perform_c") = py::cast(&Perform_c);
    ess.attr("Prepare_c") = py::cast(&Prepare_c);
    ess.attr("Manual_c") = py::cast(&Manual_c);
    ess.attr("Keystroke_c") = py::cast(&Keystroke_c);
    ess.attr("Hold_c") = py::cast(&Hold_c);
    ess.attr("Release_c") = py::cast(&Release_c);
    ess.attr("Punch_c") = py::cast(&Punch_c);
    ess.attr("Ply_c") = py::cast(&Ply_c);
    ess.attr("Point_c") = py::cast(&Point_c);
    ess.attr("Click_on_c") = py::cast(&Click_on_c);
    ess.attr("Ocular_c") = py::cast(&Ocular_c);
    ess.attr("Move_c") = py::cast(&Move_c);
    ess.attr("Look_for_c") = py::cast(&Look_for_c);
    ess.attr("Vocal_c") = py::cast(&Vocal_c);
    ess.attr("Speak_c") = py::cast(&Speak_c);
    ess.attr("Subvocalize_c") = py::cast(&Subvocalize_c);
    ess.attr("Set_mode_c") = py::cast(&Set_mode_c);
    ess.attr("Enable_c") = py::cast(&Enable_c);
    ess.attr("Disable_c") = py::cast(&Disable_c);
    ess.attr("Centering_c") = py::cast(&Centering_c);
    ess.attr("Reflex_c") = py::cast(&Reflex_c);
    ess.attr("Overt_c") = py::cast(&Overt_c);
    ess.attr("Covert_c") = py::cast(&Covert_c);
    ess.attr("Right_c") = py::cast(&Right_c);
    ess.attr("Left_c") = py::cast(&Left_c);
    ess.attr("Thumb_c") = py::cast(&Thumb_c);
    ess.attr("Index_c") = py::cast(&Index_c);
    ess.attr("Middle_c") = py::cast(&Middle_c);
    ess.attr("Ring_c") = py::cast(&Ring_c);
    ess.attr("Little_c") = py::cast(&Little_c);
    ess.attr("Motor_c") = py::cast(&Motor_c);
    ess.attr("Modality_c") = py::cast(&Modality_c);
    ess.attr("Processor_c") = py::cast(&Processor_c);
    ess.attr("Preparation_c") = py::cast(&Preparation_c);
    ess.attr("Execution_c") = py::cast(&Execution_c);
    ess.attr("Busy_c") = py::cast(&Busy_c);
    ess.attr("Free_c") = py::cast(&Free_c);
    ess.attr("Started_c") = py::cast(&Started_c);
    ess.attr("Finished_c") = py::cast(&Finished_c);
    ess.attr("Temporal_c") = py::cast(&Temporal_c);
    ess.attr("Ticks_c") = py::cast(&Ticks_c);
    ess.attr("Tag_c") = py::cast(&Tag_c);
    ess.attr("WM_c") = py::cast(&WM_c);

    py::module_ stats = m.def_submodule("statistics");
    stats.doc() = "EPICLib module containing statistical accumulators";
    py::class_<Mean_accumulator>(stats, "Mean_accumulator")
        .def(py::init<>())
        .def("reset", &Mean_accumulator::reset)
        .def("get_n", &Mean_accumulator::get_n)
        .def("get_mean", &Mean_accumulator::get_mean)
        .def("get_total", &Mean_accumulator::get_total)
        .def("get_sample_var", &Mean_accumulator::get_sample_var)
        .def("get_sample_sd", &Mean_accumulator::get_sample_sd)
        .def("get_est_var", &Mean_accumulator::get_est_var)
        .def("get_est_sd", &Mean_accumulator::get_est_sd)
        .def("get_sdm", &Mean_accumulator::get_sdm)
        .def("get_half_95_ci", &Mean_accumulator::get_half_95_ci)
        .def("update", &Mean_accumulator::update);

    py::class_<Proportion_accumulator>(stats, "Proportion_accumulator")
        .def(py::init<>())
        .def("reset", &Proportion_accumulator::reset)
        .def("get_count", &Proportion_accumulator::get_count)
        .def("get_n", &Proportion_accumulator::get_n)
        .def("get_proportion", &Proportion_accumulator::get_proportion)
        .def("update", &Proportion_accumulator::update);

    py::class_<Distribution_accumulator>(stats, "Distribution_accumulator")
        .def(py::init<int, double>())
        .def("reset", &Distribution_accumulator::reset)
        .def("get_n_bins", &Distribution_accumulator::get_n_bins)
        .def("get_bin_size", &Distribution_accumulator::get_bin_size)
        .def("get_n", &Distribution_accumulator::get_n)
        .def("get_min", &Distribution_accumulator::get_min)
        .def("get_max", &Distribution_accumulator::get_max)
        .def("get_bin_count", &Distribution_accumulator::get_bin_count)
        .def("get_bin_proportion", &Distribution_accumulator::get_bin_proportion)
        .def("get_distribution", &Distribution_accumulator::get_distribution)
        .def("update", &Distribution_accumulator::update)
        .def("add_counts", &Distribution_accumulator::add_counts);

    py::class_<Correl_accumulator>(stats, "Correl_accumulator")
        .def(py::init<>())
        .def("reset", &Correl_accumulator::reset)
        .def("get_n", &Correl_accumulator::get_n)
        .def("get_r", &Correl_accumulator::get_r)
        .def("get_slope", &Correl_accumulator::get_slope)
        .def("get_intercept", &Correl_accumulator::get_intercept)
        .def("update", &Correl_accumulator::update);

    py::module_ rms = m.def_submodule("rms_statistics");
    rms.doc() = "EPICLib module containing an rms-specific statistical accumulator";
    py::class_<Accumulate_rms_error>(rms, "Accumulate_rms_error")
        .def(py::init<>())
        .def("reset", &Accumulate_rms_error::reset)
        .def("update", &Accumulate_rms_error::update, "p1"_a, "p2"_a)
        .def("get_n", &Accumulate_rms_error::get_n)
        .def("get_rms", &Accumulate_rms_error::get_rms);
}
