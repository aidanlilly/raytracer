#include "utility.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"
#include "bvh.h"
#include "viewport.h"

#include <thread>
#include <atomic>
#include <chrono>

int main() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.img_w             = 800;
    
    int img_h = static_cast<int>(cam.img_w / cam.aspect_ratio);
    img_h = (img_h < 1) ? 1 : img_h;

    cam.samples_per_pixel = 500;
    cam.max_depth         = 10;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    Viewport viewport(cam.img_w, img_h);
    if (!viewport.ready()) return -1;

    // Allocate an 8-bit array for raw RGB pixel storage
    std::vector<unsigned char> pixelBuffer(cam.img_w * img_h * 3, 0);

    std::thread raytrace_thread([&]() {
        cam.render(world, pixelBuffer);
    });

    while (!glfwWindowShouldClose(viewport.window())) {
        // Immediate cleanup and shutdown on Escape key press
        if (glfwGetKey(viewport.window(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(viewport.window(), true);
        }

        viewport.present(pixelBuffer);

    }

    // Await background execution thread closure gracefully before destroying objects
    cam.abort_render = true;

    if (raytrace_thread.joinable()) {
        raytrace_thread.join();
    }

    return 0;
}