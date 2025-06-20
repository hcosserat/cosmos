#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <list>

#include "Particule.h"
#include "Vector2.h"
#include "random.h"

class Cosmos;

struct Astra {
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::list<Particule *> particules;
    Vector2<double> star;
    Cosmos *cosmos = nullptr;
    double m;
    std::vector<int> color;

    Astra(const char *title, const Vector2<int> &pos, const Vector2<int> &size, const std::vector<int> color);

    ~Astra();

    Vector2<double> get_other_star_pos(const Astra *other) const;

    void draw_line_to_other(const Astra *other) const;

    void clear_window() const;

    void render_present() const;

    bool is_overlapping_with_other_window(const Astra *other) const;

    Particule *new_particule(Vector2<double> v, Vector2<double> a, double m, std::vector<int> color);

    void update_particules(double dt, const Astra *other) const;

    void create_new_particules(int count);

    void draw_particules(const Astra *other) const;

    void draw(const Astra *other) const;

    void update(double dt, double p_creation_rate, Astra *other);

    void delete_distant_particules();
};

#endif //WINDOW_H
