#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera
{
public:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal_span;
	vec3 vertical_span;

public:
	camera() {
		const auto aspect_ratio = 16.0 / 9.0;
		auto viewport_height = 2.0;
		auto viewport_width = aspect_ratio * viewport_height; // viewport should be same aspect ratio as image for square pixels
		auto focal_length = 1.0;

		origin = point3(0, 0, 0);
		horizontal_span = vec3(viewport_width, 0, 0);
		vertical_span = vec3(0, viewport_height, 0);
		lower_left_corner = origin - horizontal_span / 2 - vertical_span / 2 - vec3(0, 0, focal_length);
	}

	ray get_ray(double u, double v) const
	{
		return ray(origin, lower_left_corner + u * horizontal_span + v * vertical_span - origin);
	}
};

#endif

