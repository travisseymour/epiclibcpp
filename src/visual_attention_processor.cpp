//
//  visual_attention_processor.cpp
//  Attention Classes
//
//  Originally created by Michael Walton on 1/22/14.
//  Combined-attention version; see visual_attention_processor.h and EPIC_Visual_Attention.md.
//

#include "visual_attention_processor.h"

#include "parameter.h"
#include "human_processor.h"
#include "visual_perceptual_store.h"
#include "visual_store.h"
#include "output_tee_globals.h"
#include "assert_throw.h"

#include <cmath>
#include <sstream>

using std::string;
using std::ostringstream;
using std::istringstream;
using std::endl;

/* Parameter specification:
(Eye Saliency_map <retinal_distribution> <grid_resolution> <map_radius> <cue_weight> <delay_deviation> [Debug])
*/
std::shared_ptr<Visual_attention_processor> Visual_attention_processor::create(
    const Parameter_specification& param_spec)
{
    istringstream iss(param_spec.specification);

    string distribution_name;
    iss >> distribution_name;
    if (!iss)
        Parameter::throw_parameter_error(
            "Unable to read retinal distribution in Saliency_map parameter specification", param_spec);
    Saliency_map::Retinal_distribution_e distribution;
    if (distribution_name == "Complex_Gaussian")
        distribution = Saliency_map::Complex_gaussian_retina;
    else if (distribution_name == "Gaussian")
        distribution = Saliency_map::Gaussian_retina;
    else if (distribution_name == "Flat")
        distribution = Saliency_map::Flat_retina;
    else {
        Parameter::throw_parameter_error(
            "Retinal distribution in Saliency_map parameter must be Flat, Gaussian, or Complex_Gaussian",
            param_spec);
        return nullptr;
    }

    double grid_resolution;
    iss >> grid_resolution;
    if (!iss || grid_resolution <= 0.)
        Parameter::throw_parameter_error(
            "Unable to read a positive grid resolution in Saliency_map parameter specification", param_spec);

    double radius;
    iss >> radius;
    if (!iss || radius <= 0.)
        Parameter::throw_parameter_error(
            "Unable to read a positive map radius in Saliency_map parameter specification", param_spec);

    double cue_weight;
    iss >> cue_weight;
    if (!iss || cue_weight < 0.)
        Parameter::throw_parameter_error(
            "Unable to read a non-negative cue weight in Saliency_map parameter specification", param_spec);

    double delay_deviation;
    iss >> delay_deviation;
    if (!iss || delay_deviation < 0.)
        Parameter::throw_parameter_error(
            "Unable to read a non-negative delay deviation in Saliency_map parameter specification", param_spec);

    // optional trailing debug flag: any word turns it on (e.g. Debug)
    string debug_string;
    iss >> debug_string;
    bool debug = !debug_string.empty();

    return std::make_shared<Visual_attention_processor>(
        distribution, grid_resolution, radius, cue_weight, delay_deviation, debug);
}

Visual_attention_processor::Visual_attention_processor(
    Saliency_map::Retinal_distribution_e retinal_distribution, double grid_resolution,
    double periphery_radius, double cue_weight, double delay_deviation_, bool debug_)
    : saliency_map(retinal_distribution, periphery_radius, grid_resolution, cue_weight)
    , delay_deviation(delay_deviation_)
    , debug(debug_)
    , have_last_cue(false)
{
}

void Visual_attention_processor::initialize()
{
    saliency_map.clear_cues();
    have_last_cue = false;
}

/* Rule commands */

bool Visual_attention_processor::spatial_cue(Human_processor* human_ptr, const Symbol& psychological_name)
{
    Visual_perceptual_store* vis_percept_ptr = human_ptr->get_Visual_perceptual_store_ptr();
    if (!vis_percept_ptr->is_present(psychological_name))
        return false;
    std::shared_ptr<Visual_store_object> percept_ptr = vis_percept_ptr->get_object_ptr(psychological_name);
    last_cue_location = percept_ptr->get_location();
    last_cue_size = percept_ptr->get_size();
    have_last_cue = true;
    saliency_map.add_object_cue(last_cue_location, last_cue_size);
    if (debug && Normal_out)
        Normal_out << "Saliency map: cue object " << psychological_name << "; "
                   << saliency_map.describe_cues() << endl;
    return true;
}

bool Visual_attention_processor::spatial_cue_previous()
{
    if (!have_last_cue)
        return false;
    saliency_map.add_object_cue(last_cue_location, last_cue_size);
    if (debug && Normal_out)
        Normal_out << "Saliency map: cue previous location; " << saliency_map.describe_cues() << endl;
    return true;
}

bool Visual_attention_processor::regional_cue(const Symbol& region)
{
    bool ok = saliency_map.add_regional_cue(region);
    if (ok && debug && Normal_out)
        Normal_out << "Saliency map: cue region " << region << "; " << saliency_map.describe_cues() << endl;
    return ok;
}

void Visual_attention_processor::clear_cues()
{
    saliency_map.clear_cues();
    if (debug && Normal_out)
        Normal_out << "Saliency map: cues cleared" << endl;
}

/* Delay */

long Visual_attention_processor::delay(long base_delay, std::shared_ptr<Visual_store_object> obj_ptr,
                                        GU::Point eye_location) const
{
    GU::Point obj_location = obj_ptr->get_location();
    double distance = GU::cartesian_distance(obj_location, eye_location);
    double salience = 0.;  // objects outside the map get the maximum delay
    if (distance <= saliency_map.get_periphery_radius())
        salience = saliency_map.relative_salience(obj_location, eye_location);
    double factor = 1. + delay_deviation * (1. - salience);
    long result = long(base_delay * factor + 0.5);
    if (debug && Normal_out)
        Normal_out << "Saliency map: object at " << obj_location << " eye at " << eye_location
                   << " relative salience " << salience << " delay " << base_delay << " -> " << result
                   << endl;
    return result;
}

string Visual_attention_processor::get_description() const
{
    ostringstream oss;
    oss << "Saliency map: " << saliency_map.get_description() << "; delay deviation: " << delay_deviation;
    if (debug)
        oss << "; debug output on";
    return oss.str();
}
