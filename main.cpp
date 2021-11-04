#include "rtweekend.h"

#include "colour.h"
#include "sphere.h"
#include "camera.h"
#include "hittable_list.h"

#include <vector>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iostream>

colour ray_colour(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	// stop bouncing if we've exceeded the ray bounce limit
	if (depth <= 0)
		return colour(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)) {
		point3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * ray_colour(ray(rec.p, target - rec.p), world, depth - 1);
	}

	// scale direction to unit length (-1.0 < y < 1.0)
	vec3 unit_direction = unit_vector(r.direction()); 
	// t = y component of unit_dir scaled to 0.0 <= t <= 1.0
	auto t = 0.5 * (unit_direction.y() + 1.0); 
	// linear interpolation using t
	return (1.0 - t) * colour(1.0, 1.0, 1.0) + t * colour(0.5, 0.7, 1.0); 
}

int main()
{
	// options
	const auto aspect_ratio = 16.0 / 9.0;
	const long long image_width = 400;
	const long long image_height = static_cast<int>(image_width/aspect_ratio);
	const long long upscale_factor = 1;
	const long long samples_per_pixel = 100;
	const int max_depth = 50;

	// image internal storage
	std::vector<int> pixel_array;
	pixel_array.reserve(3 * image_width * upscale_factor * image_height * upscale_factor);

	// world
	hittable_list world;
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

	// camera
	camera cam;

	// render
	auto tp1 = std::chrono::high_resolution_clock::now();
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
				pix += ray_colour(r, world, max_depth);
			}
			write_colour(pixel_array, pix, samples_per_pixel);
		}
	}
	auto tp2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_render = tp2 - tp1;

	// write to file
	std::ostringstream pixel_string;
	auto tp3 = std::chrono::high_resolution_clock::now();
	for (long long j = 0; j < image_height; ++j) {
		std::ostringstream line;
		for (long long i = 0; i < image_width; ++i) {
			for (int u = 0; u < upscale_factor; u++) {
				line << pixel_array[image_width * j * 3 + i * 3] << ' ';
				line << pixel_array[image_width * j * 3 + i * 3 + 1] << ' ';
				line << pixel_array[image_width * j * 3 + i * 3 + 2] << '\n';
			}
		}
		for(int u = 0; u < upscale_factor; u++)
			pixel_string << line.str();
	}
	auto tp4 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_string_convert = tp4 - tp3;

	auto tp5 = std::chrono::high_resolution_clock::now();
	std::ofstream file_out;
	file_out.open("out.ppm");
	file_out << "P3\n" << image_width*upscale_factor << ' ' << image_height*upscale_factor << "\n255" << std::endl;
	file_out << pixel_string.str();
	std::cerr << "\nDone" << "\n\n" << std::endl;
	file_out.close();
	auto tp6 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_file_write = tp6 - tp5;

	std::cerr << "Render time: " << time_render.count() << 's' << std::endl;
	std::cerr << "String conversion time: " << time_string_convert.count() << 's' << std::endl;
	std::cerr << "File write time: " << time_file_write.count() << 's' << std::endl;
	return 0;
}