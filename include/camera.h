#pragma once

#include "hittable.h"
#include "material.h"
#include <vector>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <functional>

class camera {
    public:
        double aspect_ratio = 1.0;
        int img_w = 100;
        int samples_per_pixel = 10;
        int max_depth = 10;

        double vfov = 90;
        point3 lookfrom = point3(0,0,0);
        point3 lookat = point3(0,0,-1);
        vec3 vup = vec3(0,1,0);

        double defocus_angle = 10.0;
        double focus_dist = 3.4;

        std::atomic<bool> abort_render = false;

        void render(const hittable& world, std::vector<unsigned char>& out_buffer) {
            initialize();
            abort_render = false;

            std::atomic<int> completed_scanlines(0);
            std::clog << "\rStarting Viewport Render with OpenMP...\n";
            auto start_time = std::chrono::high_resolution_clock::now();

            #pragma omp parallel for schedule(dynamic)
            for (int j = 0; j < img_h; j++){
                // Check if main thread signaled to abort render
                if (abort_render) continue;

                for (int i = 0; i < img_w; i++){
                    colour pixel_colour(0,0,0);
                    for (int sample = 0; sample < samples_per_pixel; sample++){
                        ray r = get_ray(i,j);
                        pixel_colour += ray_colour(r, max_depth, world);
                    }
                    
                    // Scale and apply gamma 2.0 correction
                    auto scale_color = pixel_samples_scale * pixel_colour;
                    auto r = std::sqrt(scale_color.x());
                    auto g = std::sqrt(scale_color.y());
                    auto b = std::sqrt(scale_color.z());

                    // Map floats to 0-255 bytes
                    unsigned char r_byte = static_cast<unsigned char>(256 * std::clamp(r, 0.0, 0.999));
                    unsigned char g_byte = static_cast<unsigned char>(256 * std::clamp(g, 0.0, 0.999));
                    unsigned char b_byte = static_cast<unsigned char>(256 * std::clamp(b, 0.0, 0.999));

                    // OpenGL textures start at the bottom-left corner, 
                    // so we flip the Y scanline index (img_h - 1 - j)
                    int flipped_j = img_h - 1 - j;
                    int pixel_index = (flipped_j * img_w + i) * 3;

                    out_buffer[pixel_index]     = r_byte;
                    out_buffer[pixel_index + 1] = g_byte;
                    out_buffer[pixel_index + 2] = b_byte;
                }

                int lines_done = ++completed_scanlines;
                if (lines_done % 10 == 0 || lines_done == img_h) {
                    #pragma omp critical
                    {
                        std::clog << "\rProgress: " << lines_done << " / " << img_h << " scanlines (" << (lines_done * 100 / img_h) << "%)" << std::flush;
                    }
                }
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> render_time = end_time - start_time;

            if (abort_render) {
                std::clog << "\nRender aborted by user after " << render_time.count() << " seconds.\n";
            } else {
                std::clog << "\nRender complete! Time: " << render_time.count() << " seconds\n";
            }
        }

    private:
        int img_h; 
        double pixel_samples_scale; 
        point3 centre; 
        point3 pixel00_loc;
        vec3 pixel_delta_u;
        vec3 pixel_delta_v;
        vec3 u, v, w;
        vec3 defocus_disk_u;
        vec3 defocus_disk_v;

        void initialize(){
            img_h = int(img_w / aspect_ratio);
            img_h = (img_h < 1) ? 1 : img_h;

            pixel_samples_scale = 1.0 / samples_per_pixel;
            centre = lookfrom;

            double focal_length = (lookfrom - lookat).length();
            double theta = degrees_to_radians(vfov);
            double h = std::tan(theta / 2);
            double viewport_h = 2 * h * focus_dist;
            double viewport_w = viewport_h * (double(img_w)/img_h);

            w = normalize(lookfrom - lookat);
            u = normalize(cross(vup, w));
            v = cross(w,u);
            
            vec3 viewport_u = viewport_w * u;
            vec3 viewport_v = viewport_h * -v;

            pixel_delta_u = viewport_u * (1.0/img_w);
            pixel_delta_v = viewport_v * (1.0/img_h);

            point3 viewport_upper_left = centre - (focus_dist * w) - viewport_u * (1.0 / 2) - viewport_v * (1.0 / 2);
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        ray get_ray(int i, int j){
            vec3 offset = sample_square();
            point3 pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

            point3 ray_origin = (defocus_angle <= 0) ? centre : defocus_disk_sample();
            vec3 ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        vec3 sample_square() const {
            return vec3(random_double() - 0.5, random_double() -0.5, 0);
        }

        point3 defocus_disk_sample() const {
            vec3 p = random_in_unit_disk();
            return centre + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        colour ray_colour(const ray& r, int depth, const hittable& world) const {
            if (depth <= 0){
                return colour(0,0,0);
            }
            hit_record rec;

            if (world.hit(r, interval(0.001, INF), rec)) {
                ray scattered;
                colour attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                    return attenuation * ray_colour(scattered, depth - 1, world);
                }
                return colour(0,0,0);
            }

            vec3 unit_direction = normalize(r.direction());
            double a = 0.5*(unit_direction.y() + 1.0);
            return (1.0 - a) * colour(1.0, 1.0, 1.0) + a * colour(0.5, 0.7, 1.0); 
        }
};