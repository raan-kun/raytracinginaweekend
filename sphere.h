#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
	point3 origin;
	double radius;
	shared_ptr<material> mat_ptr;

public:
	sphere() : origin(point3(0,0,-1)), radius(0.5) {}
	sphere(point3 orig, double r, shared_ptr<material> mp) : origin(orig), radius(r), mat_ptr(mp) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	vec3 o_c = r.origin() - origin;
	auto a = dot(r.direction(), r.direction());
	auto b = 2.0 * dot(r.direction(), o_c);
	auto c = dot(o_c, o_c) - (radius * radius);
	auto discriminant = b * b - 4 * a * c;

	if (discriminant < 0)
		return false;

	auto root = (-b - std::sqrt(discriminant)) / (2.0 * a);
	if (root < t_min || root > t_max) {
		root = (-b + std::sqrt(discriminant)) / (2.0 * a);
		if (root < t_min || root > t_max) {
			return false;
		}
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	vec3 outward_normal = (rec.p - origin) / radius;
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;

	return true;
}

#endif
