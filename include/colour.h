#pragma once

#include <iostream>

#include "vec3.h"
#include "interval.h"

using colour = vec3;

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) return std::sqrt(linear_component);
    return 0;
}

void write_colour(std::ostream& out, const colour& col){
    double r = col.x();
    double g = col.y();
    double b = col.z();

    // Linear to Gamma (2) space

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Convert from [0,1] to [0, 255]
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
