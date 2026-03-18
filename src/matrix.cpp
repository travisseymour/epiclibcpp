/*
 *  Matrix.cpp
 *  Visual Attention Processor
 *
 *  Created by Michael Walton on 12/15/13.
 *  Copyright (c) 2013 Michael Walton. All rights reserved.
 */

#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include <iomanip>
#include <sstream>

#include "matrix.h"

// Constructors

// No-arg
Matrix::Matrix()
    : height(0)
    , width(0)
    , max(1)
{
}

// NxN with initial value
Matrix::Matrix(size_t n, double init_val)
    : height(n)
    , width(n)
    , max(1)
{
    elements.resize(width, vector<double>(height, init_val));
    for (size_t i = 0; i < n; i++) {
        elements[i][i] = 1;
    }
}

Matrix::Matrix(size_t x, size_t y, double init_val)
    : width(x)
    , height(y)
    , max(1)
{
    elements.resize(width, vector<double>(height, init_val));
}

//------------------------------------------------------------------
// Mutators
//------------------------------------------------------------------
void Matrix::resize(size_t x, size_t y, double init_val)
{
    width = x;
    height = y;

    elements.assign(width, vector<double>(height, init_val));
}

void Matrix::set(size_t x, size_t y, double val)
{
    elements[x][y] = val;
    if (elements[x][y] > max)
        max = elements[x][y];
}

void Matrix::linear_combination(Matrix m, double scalar)
{
    assert(m.get_height() == get_height());
    assert(m.get_width() == get_width());

    max = 0;

    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] += scalar * m.get_value(x, y);
            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

void Matrix::scale(double scalar)
{
    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] *= scalar;
        }
    max = max * scalar;
}

void Matrix::convolution(Matrix kernel)
{
    double sum;
    max = 0;
    Matrix tmp = Matrix(width, height, 0.0);
    for (size_t y = 1; y + 1 < height; y++) {
        for (size_t x = 1; x + 1 < width; x++) {
            sum = 0.0;
            for (int k = -1; k <= 1; k++) {
                for (int j = -1; j <= 1; j++) {
                    sum = sum + kernel.get_value(j + 1, k + 1) * get_value(x - j, y - k);
                }
            }
            tmp.set(x, y, sum);
            if (sum > max)
                max = sum;
        }
    }
    elements = tmp.elements;
}

void Matrix::normalize(double sum)
{
    max = 0;
    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] /= sum;
            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

void Matrix::normalize()
{
    double s = this->sum();
    max = 0;

    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] /= s;
            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

void Matrix::to_gaussian(int x_mean, int y_mean, double sigma)
{
    max = 0;
    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] = exp(-0.5 * (pow((static_cast<int>(x) - x_mean) / sigma, 2.0) +
                             pow((static_cast<int>(y) - y_mean) / sigma, 2.0))) /
                             (2 * M_PI * sigma * sigma);

            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

void Matrix::to_linear(double m)
{
    max = 0;

    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] = m * static_cast<double>(x);
            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

void Matrix::to_positive_sigmoid(double upper_asymptote)
{
    max = 0;
    double x_origin = width / 2.0;

    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] =
                upper_asymptote / (1.0 + exp(-8.0 / static_cast<double>(width) * (static_cast<double>(x) - x_origin)));
            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

void Matrix::to_negative_sigmoid(double upper_asymptote)
{
    max = 0;
    double x_origin = width / 2.0;

    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] =
                upper_asymptote / (1.0 + exp(8.0 / static_cast<double>(width) * (static_cast<double>(x) - x_origin)));
            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

void Matrix::to_yaxis_sigmoid(double upper_asymptote, bool positive)
{
    max = 0;
    double y_origin = height / 2.0;
    double sign = positive ? -8.0 : 8.0;

    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] =
                upper_asymptote / (1.0 + exp(sign / static_cast<double>(height) * (static_cast<double>(y) - y_origin)));
            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

void Matrix::to_multivariable_sigmoid(double upper_asymptote, int quadrant)
{
    max = 0;
    double x_origin = width / 2.0;
    double y_origin = height / 2.0;
    double half_asymptote = upper_asymptote / 2.0;

    double x_sign, y_sign;
    switch (quadrant) {
    case 1: x_sign = -8.0; y_sign = -8.0; break;
    case 2: x_sign =  8.0; y_sign = -8.0; break;
    case 3: x_sign =  8.0; y_sign =  8.0; break;
    case 4: x_sign = -8.0; y_sign =  8.0; break;
    default: return;
    }

    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            elements[x][y] =
                half_asymptote / (1.0 + exp(x_sign / static_cast<double>(width) * (static_cast<double>(x) - x_origin))) +
                half_asymptote / (1.0 + exp(y_sign / static_cast<double>(height) * (static_cast<double>(y) - y_origin)));
            if (elements[x][y] > max)
                max = elements[x][y];
        }
}

//------------------------------------------------------------------
// Accessors
//------------------------------------------------------------------

size_t Matrix::get_height()
{
    return height;
}

size_t Matrix::get_width()
{
    return width;
}

double Matrix::get_value(size_t x, size_t y)
{
    return elements[x][y];
}

double Matrix::sum()
{
    double s = 0;
    for (size_t x = 0; x < width; ++x)
        for (size_t y = 0; y < height; ++y) {
            s += elements[x][y];
        }
    return s;
}

double Matrix::get_max()
{
    return max;
}

//------------------------------------------------------------------
// Others
//------------------------------------------------------------------

std::string Matrix::to_string()
{
    std::stringstream result;

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            if (x != 0)
                result << ",";
            result << std::setprecision(10) << elements[x][y];
        }
        result << std::endl;
    }
    return result.str();
}
