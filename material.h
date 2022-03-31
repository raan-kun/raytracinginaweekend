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
		//auto scatter_direction = rec.normal + random_unit_vector(); // cos3 distribution
		//auto scatter_direction = rec.normal + random_in_unit_sphere(); // approximate lambertian diffuse
		auto scatter_direction = random_in_hemisphere(rec.normal); // true lambertian diffuse

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

class dielectric : public material
{
public:
	double ri; // refractive index
public:
	dielectric(double refractive_index) : ri(refractive_index) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override
	{
		attenuation = colour(1.0, 1.0, 1.0);
		double refraction_ratio = rec.front_face ? (1.0 / ri) : (ri / 1.0);

		vec3 unit_direction = unit_vector(r_in.direction());
		double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = sqrt(1 - cos_theta * cos_theta);

		bool cannot_refract = (refraction_ratio * sin_theta > 1.0);
		vec3 scattered_direction;

		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
			scattered_direction = reflect(unit_direction, rec.normal); // must reflect instead
		else
			scattered_direction = refract(unit_direction, rec.normal, refraction_ratio);

		scattered = ray(rec.p, scattered_direction);
		return true;
	}
private:
	static double reflectance(double cosine, double ref_idx)
	{
		// Schlick's approximation for reflectance
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};

#endif

