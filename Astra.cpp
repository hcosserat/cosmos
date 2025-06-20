#include "Astra.h"

#include <iostream>

Astra::Astra(const char *title, const Vector2<int> &pos, const Vector2<int> &size,
             const std::vector<int> color) : star(size.x / 2.0, size.y / 2.0), color(color) {
    window = SDL_CreateWindow(title, pos.x, pos.y, size.x, size.y, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    m = 10000000;
}

Astra::~Astra() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    for (const auto p: particules) {
        delete p;
    }
}

Vector2<double> Astra::get_other_star_pos(const Astra *other) const {
    int x, y, ox, oy;
    SDL_GetWindowPosition(window, &x, &y);
    SDL_GetWindowPosition(other->window, &ox, &oy);

    const double ostarx = other->star.x + ox - x;
    const double ostary = other->star.y + oy - y;

    return {ostarx, ostary};
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

void Astra::draw_particules(const Astra *other) const {
    for (const auto p: particules) {
        SDL_SetRenderDrawColor(renderer, p->color[0], p->color[1], p->color[2], 255);
        SDL_RenderDrawPoint(renderer, static_cast<int>(p->pos.x), static_cast<int>(p->pos.y));
    }

    // if (!is_overlapping_with_other_window(other)) return;

    int x, y, ox, oy;
    SDL_GetWindowPosition(window, &x, &y);
    SDL_GetWindowPosition(other->window, &ox, &oy);

    const int dx = ox - x;
    const int dy = oy - y;

    for (const auto p: other->particules) {
        SDL_SetRenderDrawColor(renderer, p->color[0], p->color[1], p->color[2], 255);
        SDL_RenderDrawPoint(renderer, static_cast<int>(p->pos.x) + dx, static_cast<int>(p->pos.y) + dy);
    }
}

void Astra::draw(const Astra *other) const {
    clear_window();

    draw_particules(other);

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

Particule *Astra::new_particule(Vector2<double> v, Vector2<double> a = {0, 0}, const double m = 1.0,
                                const std::vector<int> color = {0, 255, 0}) {
    const auto pos = new Vector2(star.x, star.y);
    const auto p = new Particule(*pos, v, a, m, color);
    particules.push_back(p);
    return p;
}

void Astra::update_particules(const double dt, const Astra *other) const {
    for (const auto p: particules) {
        p->update(dt);

        if (is_overlapping_with_other_window(other))
            p->update_a(get_other_star_pos(other), other->m);
        else
            p->update_a(star, m / 500);
    }
}

void Astra::create_new_particules(const int count) {
    for (int i = 0; i < count; i++) {
        const auto angle = random_double(0, 2 * M_PI);
        const auto magnitude = random_double(1, 50);
        const auto v = Vector2(magnitude * cos(angle), magnitude * sin(angle));
        auto p = new_particule(v);
        int color_shift = static_cast<int>(magnitude * 2.0);
        p->color = {color[0] * color_shift / 100, color[1] * color_shift / 100, color[2] * color_shift / 100};
    }
}

void Astra::update(const double dt, const double p_creation_rate, Astra *other) {
    const int new_particules_count = p_creation_rate * dt;
    create_new_particules(new_particules_count);
    update_particules(dt, other);
    delete_distant_particules();
}

void Astra::delete_distant_particules() {
    int w;
    SDL_GetWindowSize(window, &w, nullptr);
    const double range = w * w * 4;

    particules.remove_if([this, range](const Particule *p) {
        return star.dist2(&p->pos) > range;
    });
}
