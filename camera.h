#pragma once

#include "hittable.h"
#include "material.h"
#include <vector>
#include <atomic>

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


        void render(const hittable& world){
            initialize();

            std::vector<colour> image_buffer(img_w * img_h);

            std::atomic<int> completed_scanlines(0);
            
            std::clog << "Rendering...\n";

            #pragma omp parallel for schedule(dynamic)
            for (int j = 0; j < img_h; j++){
                colour pixel_colour(0,0,0);
                for (int i = 0; i < img_w; i++){
                    colour pixel_colour(0,0,0);
                    for (int sample = 0; sample < samples_per_pixel; sample++){
                        ray r = get_ray(i,j);
                        pixel_colour += ray_colour(r, max_depth, world);
                    }
                    int pixel_index = j * img_w + i;
                    image_buffer[pixel_index] = pixel_samples_scale * pixel_colour;
                }

                int lines_done = ++completed_scanlines;

                if (lines_done % 5 == 0 || lines_done == img_h) {
                    #pragma omp critical
                    {
                        std::clog << "\rProgress: " << lines_done << " / " << img_h << " scanlines (" << (lines_done * 100 / img_h) << "%)" << std::flush;
                    }
                }
            }
            std::clog << "\rDone rendering. Writing to file...\n";

            std::cout << "P3\n" << img_w << ' ' << img_h << "\n255\n";

            for (const auto& pixel : image_buffer) {
                write_colour(std::cout, pixel);
            }

            std::clog << "Complete.\n";
        }

    private:
        int img_h; // Rendered img height
        double pixel_samples_scale; // Colour scale factor
        point3 centre; // Camera centre
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

            // Viewport dimensions

            double focal_length = (lookfrom - lookat).length();
            double theta = degrees_to_radians(vfov);
            double h = std::tan(theta / 2);
            double viewport_h = 2 * h * focus_dist;
            double viewport_w = viewport_h * (double(img_w)/img_h);

            // U, V, W basis vectors for camera frame

            w = normalize(lookfrom - lookat);
            u = normalize(cross(vup, w));
            v = cross(w,u);
            
            // Bounding vectors for viewport
        
            vec3 viewport_u = viewport_w * u;
            vec3 viewport_v = viewport_h * -v;

            // Pixel distance

            pixel_delta_u = viewport_u * (1.0/img_w);
            pixel_delta_v = viewport_v * (1.0/img_h);

            // Upper left pixel

            point3 viewport_upper_left = centre - (focus_dist * w) - viewport_u * (1.0 / 2) - viewport_v * (1.0 / 2);
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            //  Camera defocus disk basis vectors

            double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        ray get_ray(int i, int j){
            // Construct ray from origin and directed at randomly sampled points around pixel location

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