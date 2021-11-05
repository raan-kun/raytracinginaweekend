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
	camera(point3 lookfrom, point3 lookat, vec3 vup, double vfov, double aspect_ratio) { // vfov == verticle field of view in degrees
		auto theta = degrees_to_radians(vfov);
		auto h = tan(theta / 2);
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height; // viewport should be same aspect ratio as image for square pixels

		auto w = unit_vector(lookfrom - lookat);
		auto u = unit_vector(cross(vup, w));
		auto v = cross(w, u);

		auto focal_length = 1.0;

		origin = lookfrom;
		horizontal_span = viewport_width * u;
		vertical_span = viewport_height * v;
		//lower_left_corner = origin - horizontal_span / 2 - vertical_span / 2 - vec3(0, 0, focal_length);
		lower_left_corner = origin - horizontal_span / 2 - vertical_span / 2 - w;
	}

	ray get_ray(double s, double t) const
	{
		return ray(origin, lower_left_corner + s * horizontal_span + t * vertical_span - origin);
	}
};

#endif

