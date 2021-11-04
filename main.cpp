#include "rtweekend.h"

#include "colour.h"
#include "sphere.h"
#include "camera.h"
#include "hittable_list.h"

#include <iostream>
#include <sstream>
#include <fstream>

colour ray_colour(const ray& r, const hittable& world)
{
	hit_record rec;
	if (world.hit(r, 0, infinity, rec)) {
		return 0.5 * (rec.normal + colour(1, 1, 1));
	}
	//vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
	//return 0.5*colour(N.x() + 1, N.y() + 1, N.z() + 1);

	// scale direction to unit length (-1.0 < y < 1.0)
	vec3 unit_direction = unit_vector(r.direction()); 
	// t = y component of unit_dir scaled to 0.0 <= t <= 1.0
	auto t = 0.5 * (unit_direction.y() + 1.0); 
	// linear interpolation using t
	return (1.0 - t) * colour(1.0, 1.0, 1.0) + t * colour(0.5, 0.7, 1.0); 
}

int main()
{
	// image
	const auto aspect_ratio = 16.0 / 9.0;
	const long long upscale_factor = 4;
	const long long image_width = 400;
	const long long image_height = static_cast<int>(image_width/aspect_ratio);
	const long long samples_per_pixel = 100;

	// image io
	std::ofstream file_out;
	std::ostringstream pixels;

	// world
	hittable_list world;
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

	// camera
	camera cam;

	// render
	for (int j = image_height - 1; j >= 0; --j) {
		std::stringstream line;
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {
			colour pix(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				// normalise i and j & sample random point within this pixel
				auto u = (double(i) + random_double()) / (image_width - 1);
				auto v = (double(j) + random_double()) / (image_height - 1);
				// make ray for this pixel
				ray r = cam.get_ray(u, v);
				// render ray
				pix += ray_colour(r, world);
			}
			for (int u = 0; u < upscale_factor; ++u)
				write_colour(line, pix, samples_per_pixel);
		}
		for (int u = 0; u < upscale_factor; ++u)
			pixels << line.str();
	}

	file_out.open("out.ppm");
	file_out << "P3\n" << image_width*upscale_factor << ' ' << image_height*upscale_factor << "\n255" << std::endl;
	file_out << pixels.str();
	std::cerr << "\nDone" << std::endl;
	file_out.close();
	return 0;
}