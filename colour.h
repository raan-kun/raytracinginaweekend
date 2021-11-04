#pragma once

#ifndef COLOUR_H
#define COLOUR_H

#include "vec3.h"

#include <iostream>

void write_colour(std::ostream& out, colour pixel_colour, int samples_per_pixel)
{
	// vals will be >1 since we are sampling multiple times per pixel, so we must scale them back down to [0,1]
	auto scale = 1.0 / samples_per_pixel;
	
	out << static_cast<int>(255.999 * scale * pixel_colour.x()) << ' '
		<< static_cast<int>(255.999 * scale * pixel_colour.y()) << ' '
		<< static_cast<int>(255.999 * scale * pixel_colour.z()) << std::endl;
}

#endif