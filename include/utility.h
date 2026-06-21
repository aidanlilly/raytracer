#pragma once

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <memory>

// C++ std usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

inline double random_double(){
    static thread_local std::mt19937 generator;
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
    return static_cast<int>(random_double(min, max + 1));
}

// Common Headers

#include "colour.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"
