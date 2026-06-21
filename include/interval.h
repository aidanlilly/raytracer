#pragma once

#include "utility.h"

class interval {
    public:
        double min, max;
        
        interval() : min(+INF), max(-INF) {}

        interval(double min, double max) : min(min), max(max) {}

        interval(const interval& a, const interval& b) : min(std::fmin(a.min, b.min)), max(std::fmax(a.max, b.max)) {}

        double size() const { return max - min; }

        bool contains(double x) const { return min <= x && x <= max; }

        bool surrounds(double x) const { return min < x && x < max; }

        double clamp(double x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        interval expand(double delta) const {
            double padding = delta / 2;
            return interval(min - padding, max + padding);
        }

        static const interval empty, universe;
};

const interval interval::empty = interval(+INF, -INF);
const interval interval::universe = interval(-INF, +INF);