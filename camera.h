#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera
{
public:
	point3 camera_origin;
	point3 lower_left_corner;
	vec3 horizontal_span;
	vec3 vertical_span;
	vec3 u, v, w;
	double lens_radius;

public:
	// vfov == verticle field of view in degrees 
	camera(point3 lookfrom, point3 lookat, vec3 vup, double vfov, double aspect_ratio, double aperture, double focus_distance) {
		auto theta = degrees_to_radians(vfov);
		auto h = tan(theta / 2);
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height; // viewport should be same aspect ratio as image for square pixels

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		camera_origin = lookfrom;
		horizontal_span = focus_distance * viewport_width * u;
		vertical_span = focus_distance * viewport_height * v;
		lower_left_corner = camera_origin - horizontal_span / 2 - vertical_span / 2 - focus_distance * w;

		lens_radius = aperture / 2;
	}

	ray get_ray(double s, double t) const
	{
		vec3 random_in_lens = lens_radius * random_in_unit_disk();
		vec3 offset = u * random_in_lens.x() + v * random_in_lens.y();
		vec3 ray_origin = camera_origin + offset;
		return ray(ray_origin, lower_left_corner + s * horizontal_span + t * vertical_span - ray_origin);
	}
};

#endif

