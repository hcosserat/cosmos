#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <vector>
#include <functional>

#include "Particle.h"
#include "Vector2.h"
#include "random.h"

#define ASTRA_RADIUS 100  // px
#define ASTRA_MASS 1.0e7  // kg
#define MAX_PARTICLES 5000

class Cosmos;
class Particle;

struct Astra {
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::vector<Particle *> particles;
    std::vector<Particle *> core_particles; // particules qui ne quitteront pas le centre, coordonnées locales
    Vector2<double> star;
    double m;
    std::vector<int> color;
    std::vector<std::vector<int> > particle_colors;

    Astra(const char *title, const Vector2<int> &pos, const Vector2<int> &size, const std::vector<int>& color);

    ~Astra();

    Vector2<int> get_other_star_pos(const Astra *other) const;

    void clear_window() const;

    void render_present() const;

    bool is_overlapping_with_other_window(const Astra *other) const;

    Particle *new_particle(Vector2<double> v, Vector2<double> a, double m);

    void update_particles(double dt, const Astra *other) const;

    void create_new_particles(int count);

    void update(double dt, double p_creation_rate, const Astra *other);

    Vector2<double> window_to_screen(const Vector2<double> &window_pos) const;

    Vector2<int> screen_to_window(const Vector2<double> &screen_pos) const;

    Vector2<double> get_window_position() const;

    Vector2<double> get_star_screen_position() const;
};

std::function<SDL_Point(SDL_Point)> SDL_Point_converter(const Astra *src, const Astra *dest);

#endif //WINDOW_H
