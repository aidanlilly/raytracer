#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        sphere(const point3& centre, double radius, shared_ptr<material> mat) : centre(centre), radius(std::fmax(0,radius)), mat(mat) {
            vec3 rvec = vec3(radius, radius, radius);
            bbox = aabb(centre - rvec, centre + rvec);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 oc = centre - r.origin();
            double a = r.direction().length_squared();
            double h = dot(r.direction(), oc);
            double c = oc.length_squared() - radius*radius;
            double discriminant = h*h - a*c;
            
            if (discriminant < 0) {
                return false;
            }

            double sqrtd = std::sqrt(discriminant);

            double root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)){
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root)){
                    return false;
                }
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - centre) * (1.0 / radius);
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat;

            return true;
        }

        aabb bounding_box() const override { return bbox; }

    private:
        point3 centre;
        double radius;
        shared_ptr<material> mat;
        aabb bbox;
};