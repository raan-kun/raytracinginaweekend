#pragma once

#ifndef COLOUR_H
#define COLOUR_H

#include "vec3.h"

#include <vector>
#include <iostream>

void write_colour(std::vector<int>& pixels, colour pixel_colour, int samples_per_pixel)
{
	auto r = pixel_colour.x();
	auto g = pixel_colour.y();
	auto b = pixel_colour.z();

	// scale colour values to [0,1] and gamma-correct for gamma=2
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	pixels.push_back(static_cast<int>(256 * clamp(r, 0.0, 0.999)));
	pixels.push_back(static_cast<int>(256 * clamp(g, 0.0, 0.999)));
	pixels.push_back(static_cast<int>(256 * clamp(b, 0.0, 0.999)));
}

#endif