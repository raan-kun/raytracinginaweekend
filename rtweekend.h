#pragma once

#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <stdint.h>
#include "random.h"

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// RNG engine setup
std::random_device rd;
xorshift rng(rd);
std::uniform_real_distribution<double> d01(0.0, 1.0);

// Utility functions
inline double degrees_to_radians(double degrees)
{
	return degrees * pi / 180.0;
}

inline double random_double()
{
	// returns random double in [0.0, 1.0)
	return d01(rng);
}

inline double random_double(double min, double max)
{
	// returns random double in [min, max)
	std::uniform_real_distribution<double> d(min, max);
	return d(rng);
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

// Common headers
#include "ray.h"
#include "vec3.h"

#endif
