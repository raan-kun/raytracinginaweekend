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

	// vals will be >1 since we are sampling multiple times per pixel, so we must scale them back down to [0,1]
	auto scale = 1.0 / samples_per_pixel;
	r *= scale;
	g *= scale;
	b *= scale;

	pixels.push_back(static_cast<int>(256 * clamp(r, 0.0, 0.999)));
	pixels.push_back(static_cast<int>(256 * clamp(g, 0.0, 0.999)));
	pixels.push_back(static_cast<int>(256 * clamp(b, 0.0, 0.999)));
}

#endif