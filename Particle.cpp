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

void Particle::update_a_gravity(const Vector2<double> source_pos, const Vector2<double> dest_pos, const double dest_m,
                                double lin_factor) {
    /* Pour la force gravitationnelle :
     * m * a = G * m * other_m / d^2
     * => a = G * other_m / d^2
     * => â = G * other_m / d^2 * ^d/d
     *
     *
     * Pour la force supplémentaire réduisant la dispersion ("force linéaire") :
     *
     * d     p
     *   \
     *     s
     *
     * â = z * ^ds - ^p  avec z = ^p•^ds / ||ds||^2  (projection orthogonale)
     *
     */
    // Force gravitationnelle
    const double dx = dest_pos.x - pos.x;
    const double dy = dest_pos.y - pos.y;
    const double d2 = dx * dx + dy * dy;

    if (d2 < 100) {
        this->a.x = 0;
        this->a.y = 0;
        return;
    }

    const double d = sqrt(d2);
    const double k = G * dest_m / (d2 * d);
    const double f_g_x = k * dx;
    const double f_g_y = k * dy;

    // Force linéaire
    const double dsx = dest_pos.x - source_pos.x;
    const double dsy = dest_pos.y - source_pos.y;
    const double ds2 = dsx * dsx + dsy * dsy;
    if (ds2 < 100) {
        this->a.x = f_g_x;
        this->a.y = f_g_y;
        return;
    }
    const double px = pos.x - source_pos.x;
    const double py = pos.y - source_pos.y;
    const double z = (px * dsx + py * dsy) / ds2;
    const double f_l_x = z * dsx - px;
    const double f_l_y = z * dsy - py;
    const double r = 1 - sqrt((px * px + py * py) / ds2); // avancement du trajet
    const double anti_lin_factor = 4 * (r - 0.5) * (r - 0.5); // force linéaire max à la moitié du trajet,
    lin_factor *= std::max(1 - anti_lin_factor, 0.0); //    et nulle au départ et à l'arrivée

    this->a.x = lin_factor * (f_l_x - f_g_x) + f_g_x;
    this->a.y = lin_factor * (f_l_y - f_g_y) + f_g_y;
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
