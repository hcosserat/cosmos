//
// Created by Hippolyte Cosserat on 20/06/2025.
//

#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vector2.h"

class Particle {
public:
    Vector2<double> pos; // position (px)
    Vector2<double> v; // velocity (px/s)
    Vector2<double> a; // acceleration (px/s^2)
    double m; // mass (kg)
    std::vector<int> color; // color

    Particle(const Vector2<double> pos, const Vector2<double> v, const Vector2<double> a = {0, 0},
              const double m = 1.0, const std::vector<int> color = {0, 255, 0}) : pos(pos), v(v), a(a), m(m),
        color(color) {
    }

    void update(double dt);

    void update_a_gravity(Vector2<double> other_pos, double other_m);

    void update_a_fall(double k, Vector2<double> direction);
};

#endif //PARTICLE_H
