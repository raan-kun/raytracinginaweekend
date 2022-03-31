#include "rtweekend.h"

#include "colour.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "hittable_list.h"

#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iostream>

// options
const auto aspect_ratio = 16.0 / 9.0;
const long long image_width = 1280;
const long long image_height = static_cast<int>(image_width/aspect_ratio);
const long long upscale_factor = 1;
const long long samples_per_pixel = 32;
const int max_depth = 8;

// internal image buffer
std::vector<int> image_buffer(image_width * image_height * 3);
// mutex lock for image buffer
std::mutex buffer_mutex;

// for output
auto lines_remaining = image_height;

colour ray_colour(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	// stop bouncing if we've exceeded the ray bounce limit
	if (depth <= 0)
		return colour(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)) {
		ray scattered;
		colour attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_colour(scattered, world, depth - 1);

		return colour(0, 0, 0);
	}

	// sky
	// scale direction to unit length (-1.0 < y < 1.0)
	vec3 unit_direction = unit_vector(r.direction()); 
	// t = y component of unit_dir scaled to 0.0 <= t <= 1.0
	auto t = 0.5 * (unit_direction.y() + 1.0); 
	// linear interpolation using t
	return (1.0 - t) * colour(1.0, 1.0, 1.0) + t * colour(0.5, 0.7, 1.0); 
}

hittable_list random_scene()
{
	hittable_list world;

    auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = colour::random() * colour::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = colour::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

	auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

// render lines from start to end, runs per-thread
void render_lines(int start, int end, camera& cam, hittable_list& world)
{
	// allocate space for pixels this thread will render
	long long n_rows = (long long)end - (long long)start;
	std::vector<int> local_buf;
	local_buf.reserve(n_rows * image_width * 3);

	for (int j = start; j < end; ++j) {
		for (int i = 0; i < image_width; ++i) {
			colour pix(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				// normalise i and j & sample random point within this pixel
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				// make ray for this pixel
				ray r = cam.get_ray(u, v);
				// render ray
				pix += ray_colour(r, world, max_depth);
			}
			write_colour(local_buf, pix, samples_per_pixel);
		}
	}
	// write to image buffer, wait if another thread is writing
	buffer_mutex.lock();
	int i = 0;
	for (auto val : local_buf) {
		image_buffer[(long long)start*3ll*image_width + i] = val;
		i++;
	}
	lines_remaining -= n_rows;
	std::cerr << "Lines remaining: " << lines_remaining << std::endl;
	buffer_mutex.unlock();
}

int main()
{
	// world
	hittable_list world = random_scene();

	// camera
	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.2;
	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

	// thread setup
	unsigned int n_threads = std::thread::hardware_concurrency();
	std::cerr << "Using " << n_threads << " threads" << std::endl;
	std::vector<std::thread> threads;

	// launch threads!
	std::cerr << "Start render!\n" << std::endl;
	std::cerr << "Lines remaining: " << lines_remaining << std::endl;
	auto tp1 = std::chrono::high_resolution_clock::now();
	for (unsigned int i = 0; i < n_threads; ++i) {
		threads.push_back(std::thread(render_lines, i * image_height / n_threads, (i + 1ll) * image_height / n_threads, std::ref(cam), std::ref(world)));
	}
	for (std::thread& th : threads) {
		th.join();
	}
	auto tp2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_render = tp2 - tp1;
	std::cerr << "Render finished" << std::endl;

	// build output string, this is where image scaling is applied if needed
	std::cerr << "Writing to file...";
	std::ostringstream pixel_string;
	auto tp3 = std::chrono::high_resolution_clock::now();
	for (long long j = image_height - 1; j >= 0; --j) {
		std::ostringstream line;
		for (long long i = 0; i < image_width; ++i) {
			for (int u = 0; u < upscale_factor; u++) {
				line << image_buffer[image_width * j * 3 + i * 3] << ' ';
				line << image_buffer[image_width * j * 3 + i * 3 + 1] << ' ';
				line << image_buffer[image_width * j * 3 + i * 3 + 2] << '\n';
			}
		}
		for(int u = 0; u < upscale_factor; u++)
			pixel_string << line.str();
	}
	auto tp4 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_string_convert = tp4 - tp3;

	// write the file
	auto tp5 = std::chrono::high_resolution_clock::now();
	std::ofstream file_out;
	file_out.open("out.ppm");
	file_out << "P3\n" << image_width*upscale_factor << ' ' << image_height*upscale_factor << "\n255" << std::endl;
	file_out << pixel_string.str();
	std::cerr << "\nDone!" << "\n\n" << std::endl;
	file_out.close();
	auto tp6 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_file_write = tp6 - tp5;

	// output metrics
	std::cerr << "Render time: " << time_render.count() << 's' << std::endl;
	std::cerr << "String conversion time: " << time_string_convert.count() << 's' << std::endl;
	std::cerr << "File write time: " << time_file_write.count() << 's' << std::endl;
	return 0;
}