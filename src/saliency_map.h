//
//  saliency_map.h
//  Attention Classes
//
//  Originally created by Michael Walton on 1/15/14 as a grid (Matrix) based map.
//  Combined-attention version: the map is now evaluated analytically at
//  the point of interest instead of being rasterized on a grid. See
//  EPIC_Visual_Attention.md for the full description of the model.
//
//  Coordinates: all locations are EPIC visual-field coordinates in degrees of
//  visual angle (DVA), with +x to the right and +y UP. "Retinal" functions are
//  evaluated on the offset of a point from the current eye location.
//
//  The map has two layers:
//    1. A retinal (acuity) layer R, centered on the point of gaze, peak value 1.
//    2. An endogenous (cue) layer C, the sum of the cues placed by rules.
//         - Object cues are anchored in the visual field (they stay put when the eye moves).
//         - Regional cues are relative to the point of gaze (e.g. "Right" = right of fixation).
//  The combined saliency is S(p) = R(p - eye) + cue_weight * C(p).
//

#ifndef SALIENCY_MAP_H
#define SALIENCY_MAP_H

#include "geometry.h"
#include "symbol.h"

#include <list>
#include <string>

namespace GU = Geometry_Utilities;

class Saliency_map {
public:
    enum Retinal_distribution_e { Flat_retina, Gaussian_retina, Complex_gaussian_retina };

    Saliency_map(Retinal_distribution_e retinal_distribution_, double periphery_radius_,
                 double grid_resolution_, double cue_weight_);

    // cue manipulation
    // an object cue is a Gaussian centered on the location, with full-width-at-half-maximum
    // equal to the mean of the object's horizontal and vertical size
    void add_object_cue(GU::Point location, GU::Size size);
    // returns false if the region name is not recognized (and nothing is added)
    bool add_regional_cue(const Symbol& region);
    void clear_cues();
    int get_n_cues() const { return int(cues.size()); }

    // evaluation; locations are in visual-field coordinates (DVA)
    double retinal_value(GU::Point location, GU::Point eye_location) const;  // in [0, 1]
    double cue_value(GU::Point location, GU::Point eye_location) const;
    double value(GU::Point location, GU::Point eye_location) const;  // combined S
    // maximum of S over the visual field (the square of half-width periphery_radius around the eye)
    double max_value(GU::Point eye_location) const;
    // S(location) / max S, in [0, 1]; 1 at the most salient place in the field
    double relative_salience(GU::Point location, GU::Point eye_location) const;

    Retinal_distribution_e get_retinal_distribution() const { return retinal_distribution; }
    double get_periphery_radius() const { return periphery_radius; }
    double get_grid_resolution() const { return grid_resolution; }
    double get_cue_weight() const { return cue_weight; }
    std::string get_description() const;
    std::string describe_cues() const;

    // the retinal zone radii (DVA); each zone's Gaussian reaches half its peak at the zone's radius
    static const double fovea_radius_c;
    static const double parafovea_radius_c;
    static const double periphery_radius_c;

private:
    enum Cue_type_e { Object_cue, Regional_cue };
    struct Cue {
        Cue_type_e type;
        GU::Point location;  // object cues only
        double sigma;        // object cues only
        Symbol region;       // regional cues only
    };

    Retinal_distribution_e retinal_distribution;
    double periphery_radius;    // DVA; extent of the map around the point of gaze
    double grid_resolution;     // DVA; spacing of the samples used to find the maximum of the map
    double cue_weight;          // weight of the cue layer relative to the retinal layer
    double sigmoid_slope;       // per DVA, for regional cues

    std::list<Cue> cues;

    // cache of max_value(), valid for one eye location and one set of cues
    mutable bool max_cache_valid;
    mutable GU::Point max_cache_eye_location;
    mutable double max_cache_value;

    double regional_value(const Symbol& region, double dx, double dy) const;
    double sigmoid(double d) const;
};

// the symbols used to name the retinal distributions in the Saliency_map parameter
extern const Symbol Flat_c;
extern const Symbol GaussianComplex_c;
extern const Symbol Gaussian_c;

#endif
