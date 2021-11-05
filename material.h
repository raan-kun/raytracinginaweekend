#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"

class material
{
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attentuation, ray& scattered) const = 0;
};


class lambertian : public material
{
public:
	colour albedo;
public:
	lambertian(const colour& a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override
	{
		// scatter rays in random directions
		auto scatter_direction = rec.normal + random_unit_vector(); // cos3 distribution
		//auto scatter_direction = rec.normal + random_in_unit_sphere(); // approximate lambertian diffuse
		//auto scatter_direction = random_in_hemisphere(rec.normal); // true lambertian diffuse

		// catch degenerated scatter direction
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}
};

class metal : public material
{
public:
	colour albedo;
	double fuzz;
public:
	metal(const colour& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
};
#endif

