#include "Astra.h"

#include <iostream>
#ifdef __APPLE__
#include "mac_helpers.h"
#endif

Astra::Astra(const char *title, const Vector2<int> &pos, const Vector2<int> &size,
             const std::vector<int> color) : star(size.x / 2.0, size.y / 2.0), color(color) {
    window = SDL_CreateWindow(title, pos.x, pos.y, size.x, size.y, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
#ifdef __APPLE__
    ApplyMacRoundedCorners(window, 16);
#endif
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    m = ASTRA_MASS;

    constexpr int core_particles_count = 64;
    int i;

    for (i = 0; i < core_particles_count; i++) {
        const auto angle = random_double(0, 2 * M_PI);
        const auto magnitude = random_double(1, ASTRA_RADIUS / 8.0);
        const auto v = Vector2(magnitude * cos(angle), magnitude * sin(angle));
        core_particles.push_back(new Particle(star, v, {0, 0}, 1));
    }

    for (i = 0; i < 10; i++) {
        const double factor = i / 9.0;
        const auto r = static_cast<Uint8>(color[0] + (255 - color[0]) * factor);
        const auto g = static_cast<Uint8>(color[1] + (255 - color[1]) * factor);
        const auto b = static_cast<Uint8>(color[2] + (255 - color[2]) * factor);
        std::vector<int> particle_color{r, g, b};
        particle_colors.push_back(particle_color);
    }
}


Astra::~Astra() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    for (const auto p: particles) {
        delete p;
    }
    for (const auto p: core_particles) {
        delete p;
    }
}

Vector2<double> Astra::get_window_position() const {
    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    return {static_cast<double>(x), static_cast<double>(y)};
}

Vector2<double> Astra::window_to_screen(const Vector2<double> &window_pos) const {
    const Vector2<double> window_origin = get_window_position();
    return {window_pos.x + window_origin.x, window_pos.y + window_origin.y};
}

Vector2<int> Astra::screen_to_window(const Vector2<double> &screen_pos) const {
    const Vector2<double> window_origin = get_window_position();
    return (screen_pos - window_origin).as_int();
}

Vector2<double> Astra::get_star_screen_position() const {
    return window_to_screen(star);
}

Vector2<int> Astra::get_other_star_pos(const Astra *other) const {
    return screen_to_window(other->get_star_screen_position());
}

void Astra::clear_window() const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Astra::render_present() const {
    SDL_RenderPresent(renderer);
}

bool Astra::is_overlapping_with_other_window(const Astra *other) const {
    // Au sens large
    int x, y, w, h, ox, oy, ow, oh;
    SDL_GetWindowPosition(window, &x, &y);
    SDL_GetWindowSize(window, &w, &h);
    SDL_GetWindowPosition(other->window, &ox, &oy);
    SDL_GetWindowSize(other->window, &ow, &oh);

    if (ox + ow + 1 < x) return false;
    if (ox > x + w + 1) return false;

    if (oy + oh + 1 < y) return false;
    if (oy > y + h + 1) return false;

    return true;
}

Particle *Astra::new_particle(const Vector2<double> v, const Vector2<double> a = {0, 0}, const double m = 1.0) {
    const Vector2<double> screen_pos = get_star_screen_position();
    const auto p = new Particle(screen_pos, v, a, m);
    particles.push_back(p);
    return p;
}

void Astra::update_particles(const double dt, const Astra *other) const {
    const Vector2<double> this_star_screen_pos = get_star_screen_position();
    const auto noise = Vector2(
        random_double(-ASTRA_RADIUS / 5.0, ASTRA_RADIUS / 5.0),
        random_double(-ASTRA_RADIUS / 5.0, ASTRA_RADIUS / 5.0)
    );

    for (const auto p: particles) {
        if (is_overlapping_with_other_window(other)) {
            Vector2<double> other_star_screen_pos = other->get_star_screen_position() + noise * 4;

            if (other_star_screen_pos.dist2(&p->pos) >= ASTRA_RADIUS * ASTRA_RADIUS)
                p->update_a_gravity_2(this_star_screen_pos, other_star_screen_pos, other->m, 0.5);
            else
                p->update_a_gravity(other_star_screen_pos, this_star_screen_pos, this->m);
        } else {
            p->update_a_fall(10, this_star_screen_pos);
        }
        p->update(dt);
    }

    for (const auto p: core_particles) {
        p->update_a_fall(50, star + noise);
        p->update(dt * 5.0); // simule des particules plus rapides

        if (p->v.norm2() > 1000) {
            p->v *= 0.9;
            p->a *= 0.7;
        }
    }
}

void Astra::create_new_particles(const int count) {
    for (int i = 0; i < count; i++) {
        // If we've reached the maximum number of particles, remove the oldest ones
        if (particles.size() >= MAX_PARTICLES) {
            // Calculate how many particles need to be removed
            size_t to_remove = 1; // At minimum, remove one to make space

            // Delete the oldest particles (which are at the beginning of the vector)
            for (size_t j = 0; j < to_remove && !particles.empty(); j++) {
                delete particles[j]; // Free the memory
            }

            // Remove the pointers from the vector
            particles.erase(particles.begin(), particles.begin() + std::min(to_remove, particles.size()));
        }

        // Create the new particle
        const auto angle = random_double(0, 2 * M_PI);
        const auto magnitude = random_double(1, ASTRA_RADIUS / 2.0);
        const auto v = Vector2(magnitude * cos(angle), magnitude * sin(angle));
        new_particle(v);
    }
}

void Astra::update(const double dt, const double p_creation_rate, const Astra *other) {
    const int new_particles_count = p_creation_rate * dt;
    create_new_particles(new_particles_count);
    update_particles(dt, other);
}

std::function<SDL_Point(SDL_Point)> SDL_Point_converter(const Astra *src, const Astra *dest) {
    return [src, dest](const SDL_Point pt) {
        const auto screen_pos = src->window_to_screen(Vector2<double>(pt.x, pt.y));
        const auto new_window_pos = dest->screen_to_window(screen_pos);
        return new_window_pos.as_SDL_Point();
    };
}
