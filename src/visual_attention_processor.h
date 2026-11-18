//
//  visual_attention_processor.h
//  Attention Classes
//
//  Originally created by Michael Walton on 1/22/14.
//  Combined-attention version; see EPIC_Visual_Attention.md.
//
//  The Visual_attention_processor owns the saliency map and converts the relative
//  salience of an object's location into a lengthening of a property's transduction delay:
//
//      delay = base_delay * (1 + delay_deviation * (1 - S(object) / max S))
//
//  so the most salient place in the visual field gets the base delay, and the least
//  salient places get up to (1 + delay_deviation) times the base delay.
//  Objects farther than the map radius from the point of gaze get the maximum delay.
//
//  The Eye_processor only creates one of these when a Saliency_map parameter is given;
//  without it, no saliency modulation happens at all.
//

#ifndef VISUAL_ATTENTION_PROCESSOR_H
#define VISUAL_ATTENTION_PROCESSOR_H

#include "saliency_map.h"
#include "symbol.h"
#include "geometry.h"

#include <memory>
#include <string>

namespace GU = Geometry_Utilities;

class Human_processor;
class Visual_store_object;
struct Parameter_specification;

class Visual_attention_processor {
public:
    Visual_attention_processor(Saliency_map::Retinal_distribution_e retinal_distribution,
                               double grid_resolution, double periphery_radius, double cue_weight,
                               double delay_deviation, bool debug);

    // parse a Saliency_map parameter specification:
    // <retinal_distribution> <grid_resolution> <map_radius> <cue_weight> <delay_deviation> [Debug]
    static std::shared_ptr<Visual_attention_processor> create(const Parameter_specification& param_spec);

    // reset the per-run state (all cues and the remembered cue location)
    void initialize();

    // rule commands
    // cue the location of a visual object, named by its psychological name; returns false if
    // the object is not in the visual perceptual store
    bool spatial_cue(Human_processor* human_ptr, const Symbol& psychological_name);
    // cue the last cued object location again; returns false if there was none
    bool spatial_cue_previous();
    // returns false if the region name is not recognized
    bool regional_cue(const Symbol& region);
    void clear_cues();

    // the saliency-modulated delay for a property of an object
    long delay(long base_delay, std::shared_ptr<Visual_store_object> obj_ptr, GU::Point eye_location) const;

    const Saliency_map& get_saliency_map() const { return saliency_map; }
    double get_delay_deviation() const { return delay_deviation; }
    bool get_debug() const { return debug; }
    std::string get_description() const;

private:
    Saliency_map saliency_map;
    double delay_deviation;
    bool debug;

    bool have_last_cue;
    GU::Point last_cue_location;
    GU::Size last_cue_size;
};

#endif
