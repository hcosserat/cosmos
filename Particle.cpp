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
    const auto d = dest_pos - pos;
    const double d2 = d.norm2();

    if (d2 < 100) {
        this->a.x = 0;
        this->a.y = 0;
        return;
    }

    const double d_norm = sqrt(d2);
    const double k = G * dest_m / (d2 * d_norm);
    const auto f_g = k * d;

    // Force linéaire
    const auto sd = dest_pos - source_pos;
    const double sd2 = sd.norm2();

    if (sd2 < 100) {
        // Deux astres trop proches, pas de force linéaire
        this->a = f_g;
        return;
    }

    const auto sp = pos - source_pos;
    const double sp2 = sp.norm2();

    if (sp2 < 100) {
        // Particule trop proche de la source, pas de force linéaire
        this->a = f_g;
        return;
    }

    const double sp_dot_sd = sp.dot(&sd);

    const double z = sp_dot_sd / sd2;

    auto f_l = z * (sd - sp);

    const double sp_norm = sqrt(sp2);
    const double sd_norm = sqrt(sd2);
    const double cos_sp_sd = sp_dot_sd / (sp_norm * sd_norm);
    const double r = sp_norm / sd_norm; // avancement du trajet

    if (cos_sp_sd < 0.05 || (cos_sp_sd > 0.95 && r < 0.5) || r <= 0 || r >= 1) {
        // Particule presque "derrière" la source
        // ou déjà proche de (sd)
        // ou derrière la destination, pas de force linéaire
        this->a = f_g;
        return;
    }

    lin_factor *= cos_sp_sd;

    const double x = r - 0.5;
    const double dist_lin_factor = -M_2_PI * atan(10 * x) * (1 - x * x);

    f_l *= dist_lin_factor;

    this->a = lin_factor * (f_l - f_g) + f_g;
}


void Particle::update_a_gravity_2(const Vector2<double> &source_pos, const Vector2<double> &dest_pos,
                                  const double dest_m, const double lin_factor) {
    constexpr double source_radius = 50; // No acceleration zone around source
    constexpr double dest_radius = 150; // Distance to start wrapping behavior
    const double tunnel_strength = lin_factor * 4; // Strength of the tunnel effect

    // 1. Near source: zero acceleration
    const auto sp = pos - source_pos;
    const double sp_norm = sqrt(sp.norm2());
    if (sp_norm < source_radius) {
        this->a.x = 0;
        this->a.y = 0;
        return;
    }

    // Calculate vector to destination
    const auto d = dest_pos - pos;
    const double d2 = d.norm2();
    const double d_norm = sqrt(d2);

    // Source to destination vector
    const auto sd = dest_pos - source_pos;
    const double sd_norm = sqrt(sd.norm2());
    Vector2<double> sd_unit;
    if (sd_norm > 0) {
        sd_unit = sd * (1.0 / sd_norm);
    } else {
        sd_unit = {0, 0};
    }

    // 2. Gravitational force toward destination
    const double k = d2 > 10 ? G * dest_m / (d2 * d_norm) : 0;
    const auto f_g = d * k;

    // Progress along path (0 at source, 1 at destination)
    double progress = sp.dot(&sd) / (sd_norm * sd_norm);
    progress = std::min(std::max(progress, 0.0), 1.0);

    // 3. Tunnel effect: force toward the central line
    // Project current position onto the source-destination line
    const auto proj = source_pos + sd_unit * (progress * sd_norm);
    const auto lateral_offset = proj - pos;
    const double lateral_dist = sqrt(lateral_offset.norm2());

    // Calculate tunnel force
    constexpr double tunnel_peak = 0.4;
    const double tunnel_profile = progress / tunnel_peak * (2 - progress / tunnel_peak);
    const auto lateral_dir = lateral_dist > 30
                                 ? lateral_offset / lateral_dist
                                 : lateral_dist > 0
                                       ? - lateral_offset / lateral_dist
                                       : Vector2<double>(0, 0);
    const auto f_tunnel = lateral_dir * tunnel_strength * tunnel_profile * lateral_dist;

    // 4. Destination wrapping behavior
    if (d_norm < dest_radius) {
        // Calculate angle from destination to determine wrap direction
        const double wrap_strength = 1.0 - d_norm / dest_radius; // Stronger as we get closer

        // Create perpendicular force to wrap around destination
        auto perp_dir = Vector2(-d.y, d.x) * spin;
        const double perp_norm = sqrt(perp_dir.norm2());

        if (perp_norm > 0) {
            perp_dir = perp_dir * (1.0 / perp_norm);
        }

        const auto f_wrap = perp_dir * (wrap_strength * sqrt(f_g.norm2()));

        // Combine with other forces
        this->a = f_g + f_wrap;
        return;
    }

    // Normal case: combine gravitational and tunnel forces
    // Adjust the balance based on progress
    const double tunnel_weight = std::min(std::max(1.0 - 10.0 * fabs(progress - tunnel_peak) + 3, 0.2), 1.0);
    constexpr double gravity_weight = 1.0;

    this->a = f_tunnel * tunnel_weight + f_g * gravity_weight;
}


void Particle::update_a_fall(double k, const Vector2<double> direction) {
    // â = k * ^d
    const auto d = direction - pos;
    const double d2 = d.norm2();
    const double d_norm = sqrt(d2);
    k = d_norm > 10 ? k / d_norm : 0;

    this->a = k * d;
}
