//
// Created by Hippolyte Cosserat on 20/06/2025.
//

#include "Particle.h"

#define G 1

void Particle::update(const double dt) {
    v += dt * a;

    constexpr double max_speed = 200;
    const double v_magnitude_squared = v.x * v.x + v.y * v.y;

    if (v_magnitude_squared > max_speed * max_speed) {
        const double scale = max_speed / sqrt(v_magnitude_squared);
        v = v * scale;
    }

    pos += dt * v;
}

void Particle::update_a_gravity(const Vector2<double> other_pos, const double other_m) {
    // m * a = G * m * other_m / d^2
    // => a = G * other_m / d^2
    // => â = G * other_m / d^2 * ^d/d
    const double dx = other_pos.x - pos.x;
    const double dy = other_pos.y - pos.y;
    const double d2 = dx * dx + dy * dy;

    if (d2 < 100) {
        this->a.x = 0;
        this->a.y = 0;
        return;
    }

    const double d = sqrt(d2);
    const double k = G * other_m / (d2 * d);
    this->a.x = k * dx;
    this->a.y = k * dy;
}

void Particle::update_a_fall(double k, const Vector2<double> direction) {
    // â = k * ^d
    const double dx = direction.x - pos.x;
    const double dy = direction.y - pos.y;
    const double d = sqrt(dx * dx + dy * dy);
    k = d > 10 ? k / d : 0;

    this->a.x = k * dx;
    this->a.y = k * dy;
}
