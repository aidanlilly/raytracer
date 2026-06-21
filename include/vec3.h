#pragma once

#include <cmath>

class vec3 {
    public:

        double e[3];
        
        vec3() : e{0,0,0} {}
        vec3(double x, double y, double z) : e{x, y, z}{}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        double operator[](int n) const { return e[n]; }
        double& operator[](int n){ return e[n]; }
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

        vec3 operator*=(double n) { 
            e[0] *= n;
            e[1] *= n;
            e[2] *= n;
            return *this;
        }

        vec3 operator+=(vec3 v) { 
            e[0] += v.x();
            e[1] += v.y();
            e[2] += v.z();
            return *this;
        }

        double length() const {
            return std::sqrt(length_squared());
        }

        double length_squared() const {
            return (e[0] * e[0]) + (e[1] * e[1]) + (e[2] * e[2]);
        }

        bool near_zero() const {
            double s = 1e-8;
            return (std::fabs(e[0] < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2] < 2)));
        }

        static vec3 random() {
            return vec3(random_double(), random_double(), random_double());
        }

        static vec3 random(double min, double max) {
            return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
        }
};

// Alias for geometric clarity
using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v){
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3& v, double n){
    return vec3(v.e[0] * n, v.e[1] * n, v.e[2] * n);
}

inline vec3 operator*(double n, const vec3& v){
    return vec3(v.e[0] * n, v.e[1] * n, v.e[2] * n);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline double dot(const vec3& v1, const vec3& v2){
    return (v1.e[0] * v2.e[0]) + (v1.e[1] * v2.e[1]) + (v1.e[2] * v2.e[2]);
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
                v1.e[2] * v2.e[0] - v1.e[0] * v2.e[2],
                v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline vec3 normalize(const vec3& v){
    return v * (1.0/v.length());
}

inline vec3 random_in_unit_disk() {
    while (true) {
        vec3 p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() < 1) return p;
    }
}

inline vec3 random_unit_vector() {
    while (true) {
        vec3 p = vec3::random(-1, 1);
        double lensq = p.length_squared();
        if ( 1e-160 < lensq && lensq <= 1 ) {
            return p * (1.0 / sqrt(lensq));
        }
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) return on_unit_sphere;
    else return -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v -2*dot(v,n) * n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    double cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}