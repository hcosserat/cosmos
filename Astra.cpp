#include "Astra.h"

#include <iostream>

Astra::Astra(const char *title, const Vector2<int> &pos, const Vector2<int> &size,
             const std::vector<int> color) : star(size.x / 2.0, size.y / 2.0), color(color) {
    window = SDL_CreateWindow(title, pos.x, pos.y, size.x, size.y, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    m = ASTRA_MASS;
}

Astra::~Astra() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    for (const auto p: particles) {
        delete p;
    }
}

Vector2<double> Astra::get_window_position() const {
    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    return {static_cast<double>(x), static_cast<double>(y)};
}

Vector2<double> Astra::window_to_screen(const Vector2<double> &window_pos) const {
    Vector2<double> window_origin = get_window_position();
    return {window_pos.x + window_origin.x, window_pos.y + window_origin.y};
}

Vector2<double> Astra::screen_to_window(const Vector2<double> &screen_pos) const {
    Vector2<double> window_origin = get_window_position();
    return {screen_pos.x - window_origin.x, screen_pos.y - window_origin.y};
}

Vector2<double> Astra::get_star_screen_position() const {
    return window_to_screen(star);
}

Vector2<double> Astra::get_other_star_pos(const Astra *other) const {
    // Get the screen position of the other star
    Vector2<double> other_screen_pos = other->get_star_screen_position();
    // Convert to this window's coordinates
    return screen_to_window(other_screen_pos);
}

void Astra::draw_line_to_other(const Astra *other) const {
    // Coordonnées de l'autre étoile dans notre référentiel
    const Vector2<double> other_star_pos = get_other_star_pos(other);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawLine(renderer, star.x, star.y, other_star_pos.x, other_star_pos.y);
}

void Astra::clear_window() const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Astra::render_present() const {
    SDL_RenderPresent(renderer);
}

void Astra::draw_particles(const Astra *other) const {
    // Draw own particles
    for (const auto p: particles) {
        // Convert particle's screen position to window coordinates
        Vector2<double> window_pos = screen_to_window(p->pos);
        SDL_SetRenderDrawColor(renderer, p->color[0], p->color[1], p->color[2], 255);
        SDL_RenderDrawPoint(renderer, static_cast<int>(window_pos.x), static_cast<int>(window_pos.y));
    }

    // Draw other window's particles
    for (const auto p: other->particles) {
        // Convert particle's screen position to window coordinates
        Vector2<double> window_pos = screen_to_window(p->pos);
        SDL_SetRenderDrawColor(renderer, p->color[0], p->color[1], p->color[2], 255);
        SDL_RenderDrawPoint(renderer, static_cast<int>(window_pos.x), static_cast<int>(window_pos.y));
    }
}

void Astra::draw(const Astra *other) const {
    clear_window();

    draw_particles(other);

    render_present();
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

Particle *Astra::new_particle(Vector2<double> v, Vector2<double> a = {0, 0}, const double m = 1.0,
                                const std::vector<int> color = {0, 255, 0}) {
    // Create particle at star's screen position
    const Vector2<double> screen_pos = get_star_screen_position();
    const auto p = new Particle(screen_pos, v, a, m, color);
    particles.push_back(p);
    return p;
}

void Astra::update_particles(const double dt, const Astra *other) const {
    for (const auto p: particles) {
        p->update(dt);

        // Get the screen positions of stars for gravity calculations
        Vector2<double> this_star_screen_pos = get_star_screen_position();
        Vector2<double> other_star_screen_pos = other->get_star_screen_position();

        if (is_overlapping_with_other_window(other)) {
            const auto other_star_screen_pos_noise = Vector2(
                random_double(-ASTRA_RADIUS / 5.0, ASTRA_RADIUS / 5.0),
                random_double(-ASTRA_RADIUS / 5.0, ASTRA_RADIUS / 5.0)
            );
            other_star_screen_pos += other_star_screen_pos_noise;

            const double dist2 = other_star_screen_pos.dist2(&p->pos);

            if (dist2 >= ASTRA_RADIUS * ASTRA_RADIUS)
            p->update_a_gravity(other_star_screen_pos, other->m);
            else
            p->update_a_gravity(this_star_screen_pos, m);
        } else {
            p->update_a_fall(m * 1.0e-6, this_star_screen_pos);
        }
    }
}

void Astra::create_new_particles(const int count) {
    // Rest of the function remains unchanged
    for (int i = 0; i < count; i++) {
        const auto angle = random_double(0, 2 * M_PI);
        const auto magnitude = random_double(1, ASTRA_RADIUS / 2.0);
        const auto v = Vector2(magnitude * cos(angle), magnitude * sin(angle));
        auto p = new_particle(v);
        int color_shift = static_cast<int>(magnitude * 2.0);
        p->color = {color[0] * color_shift / 100, color[1] * color_shift / 100, color[2] * color_shift / 100};
    }
}

void Astra::update(const double dt, const double p_creation_rate, const Astra *other) {
    const int new_particles_count = p_creation_rate * dt;
    create_new_particles(new_particles_count);
    update_particles(dt, other);
    delete_distant_particles();
}

void Astra::delete_distant_particles() {
    int w;
    SDL_GetWindowSize(window, &w, nullptr);
    const double range = w * w * 4;

    // Get star's screen position for distance calculation
    Vector2<double> star_screen_pos = get_star_screen_position();

    particles.remove_if([&star_screen_pos, range](const Particle *p) {
        const double dx = p->pos.x - star_screen_pos.x;
        const double dy = p->pos.y - star_screen_pos.y;
        return dx * dx + dy * dy > range;
    });
}
