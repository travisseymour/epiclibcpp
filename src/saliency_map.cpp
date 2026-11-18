//
//  saliency_map.cpp
//  Attention Classes
//
//  Originally created by Michael Walton on 1/15/14.
//  Combined-attention version: analytic evaluation; see saliency_map.h
//  and EPIC_Visual_Attention.md.
//

#include "saliency_map.h"
#include "epic_standard_symbols.h"
#include "assert_throw.h"

#include <cmath>
#include <sstream>

using std::exp;
using std::sqrt;
using std::string;
using std::ostringstream;

const Symbol Flat_c("Flat");
const Symbol GaussianComplex_c("Complex_Gaussian");
const Symbol Gaussian_c("Gaussian");

const double Saliency_map::fovea_radius_c = 1.0;
const double Saliency_map::parafovea_radius_c = 7.5;
const double Saliency_map::periphery_radius_c = 60.0;

// full width at half maximum = 2 sqrt(2 ln 2) sigma
static const double FWHM_per_sigma_c = 2.354820045;

// Gaussian with peak 1
static double gaussian(double dx, double dy, double sigma)
{
    return exp(-0.5 * (dx * dx + dy * dy) / (sigma * sigma));
}

// sigma of a Gaussian whose value falls to half its peak at the given radius
static double sigma_for_half_max_radius(double radius)
{
    return (2. * radius) / FWHM_per_sigma_c;
}

Saliency_map::Saliency_map(Retinal_distribution_e retinal_distribution_, double periphery_radius_,
                            double grid_resolution_, double cue_weight_)
    : retinal_distribution(retinal_distribution_)
    , periphery_radius(periphery_radius_)
    , grid_resolution(grid_resolution_)
    , cue_weight(cue_weight_)
    , max_cache_valid(false)
    , max_cache_value(1.)
{
    Assert(periphery_radius > 0.);
    Assert(grid_resolution > 0.);
    Assert(cue_weight >= 0.);
    // regional cues: the same shape as the original grid implementation,
    // a logistic that rises over the half-width of the map: slope = 4 / periphery_radius per DVA
    sigmoid_slope = 4. / periphery_radius;
}

/* Cues */

void Saliency_map::add_object_cue(GU::Point location, GU::Size size)
{
    double fwhm = (size.h + size.v) / 2.;
    // a point-sized object still gets a cue at least one grid step wide
    if (fwhm < grid_resolution)
        fwhm = grid_resolution;
    Cue cue;
    cue.type = Object_cue;
    cue.location = location;
    cue.sigma = fwhm / FWHM_per_sigma_c;
    cues.push_back(cue);
    max_cache_valid = false;
}

bool Saliency_map::add_regional_cue(const Symbol& region)
{
    if (!(region == Right_c || region == Left_c || region == Above_c || region == Below_c ||
          region == Upper_Right_c || region == Upper_Left_c || region == Lower_Left_c ||
          region == Lower_Right_c || region == Visual_Space_c))
        return false;
    Cue cue;
    cue.type = Regional_cue;
    cue.sigma = 0.;
    cue.region = region;
    cues.push_back(cue);
    max_cache_valid = false;
    return true;
}

void Saliency_map::clear_cues()
{
    cues.clear();
    max_cache_valid = false;
}

/* Evaluation */

double Saliency_map::sigmoid(double d) const
{
    return 1. / (1. + exp(-sigmoid_slope * d));
}

// dx, dy are the offset from the point of gaze, +y up; value in [0, 1]
double Saliency_map::regional_value(const Symbol& region, double dx, double dy) const
{
    if (region == Right_c)
        return sigmoid(dx);
    if (region == Left_c)
        return sigmoid(-dx);
    if (region == Above_c)
        return sigmoid(dy);
    if (region == Below_c)
        return sigmoid(-dy);
    if (region == Upper_Right_c)
        return (sigmoid(dx) + sigmoid(dy)) / 2.;
    if (region == Upper_Left_c)
        return (sigmoid(-dx) + sigmoid(dy)) / 2.;
    if (region == Lower_Left_c)
        return (sigmoid(-dx) + sigmoid(-dy)) / 2.;
    if (region == Lower_Right_c)
        return (sigmoid(dx) + sigmoid(-dy)) / 2.;
    // Visual_Space: the whole field equally
    return 1.;
}

double Saliency_map::retinal_value(GU::Point location, GU::Point eye_location) const
{
    double dx = location.x - eye_location.x;
    double dy = location.y - eye_location.y;
    switch (retinal_distribution) {
        case Gaussian_retina:
            return gaussian(dx, dy, sigma_for_half_max_radius(parafovea_radius_c));
        case Complex_gaussian_retina:
            // equal-peak mixture of foveal, parafoveal, and peripheral Gaussians, scaled to peak 1
            return (gaussian(dx, dy, sigma_for_half_max_radius(fovea_radius_c)) +
                    gaussian(dx, dy, sigma_for_half_max_radius(parafovea_radius_c)) +
                    gaussian(dx, dy, sigma_for_half_max_radius(periphery_radius_c))) /
                   3.;
        case Flat_retina:
        default:
            return 1.;
    }
}

double Saliency_map::cue_value(GU::Point location, GU::Point eye_location) const
{
    double sum = 0.;
    for (std::list<Cue>::const_iterator it = cues.begin(); it != cues.end(); ++it) {
        if (it->type == Object_cue)
            sum += gaussian(location.x - it->location.x, location.y - it->location.y, it->sigma);
        else
            sum += regional_value(it->region, location.x - eye_location.x, location.y - eye_location.y);
    }
    return sum;
}

double Saliency_map::value(GU::Point location, GU::Point eye_location) const
{
    return retinal_value(location, eye_location) + cue_weight * cue_value(location, eye_location);
}

double Saliency_map::max_value(GU::Point eye_location) const
{
    if (max_cache_valid && max_cache_eye_location == eye_location)
        return max_cache_value;

    // sample the square field around the eye on the grid, plus the exact peaks of the components
    double max = value(eye_location, eye_location);
    for (std::list<Cue>::const_iterator it = cues.begin(); it != cues.end(); ++it) {
        if (it->type != Object_cue)
            continue;
        if (std::fabs(it->location.x - eye_location.x) > periphery_radius ||
            std::fabs(it->location.y - eye_location.y) > periphery_radius)
            continue;
        double v = value(it->location, eye_location);
        if (v > max)
            max = v;
    }
    int n = int(periphery_radius / grid_resolution);
    for (int i = -n; i <= n; i++) {
        for (int j = -n; j <= n; j++) {
            GU::Point p(eye_location.x + i * grid_resolution, eye_location.y + j * grid_resolution);
            double v = value(p, eye_location);
            if (v > max)
                max = v;
        }
    }
    max_cache_valid = true;
    max_cache_eye_location = eye_location;
    max_cache_value = max;
    return max;
}

double Saliency_map::relative_salience(GU::Point location, GU::Point eye_location) const
{
    double max = max_value(eye_location);
    if (max <= 0.)
        return 1.;
    double s = value(location, eye_location) / max;
    if (s > 1.)  // a location between grid samples can slightly exceed the sampled maximum
        s = 1.;
    if (s < 0.)
        s = 0.;
    return s;
}

string Saliency_map::get_description() const
{
    ostringstream oss;
    oss << "retinal distribution: ";
    switch (retinal_distribution) {
        case Gaussian_retina:
            oss << Gaussian_c;
            break;
        case Complex_gaussian_retina:
            oss << GaussianComplex_c;
            break;
        default:
            oss << Flat_c;
            break;
    }
    oss << "; map radius: " << periphery_radius << " DVA; grid resolution: " << grid_resolution
        << " DVA; cue weight: " << cue_weight;
    return oss.str();
}

string Saliency_map::describe_cues() const
{
    ostringstream oss;
    if (cues.empty())
        return "no cues";
    bool first = true;
    for (std::list<Cue>::const_iterator it = cues.begin(); it != cues.end(); ++it) {
        if (!first)
            oss << ", ";
        first = false;
        if (it->type == Object_cue)
            oss << "object cue at " << it->location << " FWHM " << it->sigma * FWHM_per_sigma_c;
        else
            oss << "region " << it->region;
    }
    return oss.str();
}
