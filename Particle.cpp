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
     * â = z * ^sd - ^sp  avec z = ^sp•^sd / ||sd||^2  (projection orthogonale)
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
    const double sdx = dest_pos.x - source_pos.x;
    const double sdy = dest_pos.y - source_pos.y;
    const double sd2 = sdx * sdx + sdy * sdy;

    if (sd2 < 100) {
        // Deux astres trop proches, pas de force linéaire
        this->a.x = f_g_x;
        this->a.y = f_g_y;
        return;
    }

    const double spx = pos.x - source_pos.x;
    const double spy = pos.y - source_pos.y;
    const double sp_dot_sd = spx * sdx + spy * sdy;
    const double z = sp_dot_sd / sd2;
    const double f_l_x = z * sdx - spx;
    const double f_l_y = z * sdy - spy;
    const double sp2 = spx * spx + spy * spy;

    if (sp2 < 100) {
        // Particule trop proche de la source, pas de force linéaire
        this->a.x = f_g_x;
        this->a.y = f_g_y;
        return;
    }

    const double sp = sqrt(sp2);
    const double sd = sqrt(sd2);
    const double cos_sp_sd = sp_dot_sd / (sp * sd);
    const double r = sp / sd; // avancement du trajet

    if (cos_sp_sd < 0.05 || (cos_sp_sd > 0.95 && r < 0.6) || r <= 0 || r >= 1) {
        // Particule presque "derrière" la source
        // ou déjà proche de sd
        // ou derrière la destination, pas de force linéaire
        this->a.x = f_g_x;
        this->a.y = f_g_y;
        return;
    }

    const double dist_lin_factor =
            r < 0.5
                ? (0.5 + (10 - 14 * r) * r) * r
                : r < 0.75
                      ? -55 + (288 + (-480 + 256 * r) * r) * r
                      : 84.5 + (-304.5 + (356 - 136 * r) * r) * r;

    lin_factor *= dist_lin_factor;

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
