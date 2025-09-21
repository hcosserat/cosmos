//
// Created by Hippolyte Cosserat on 19/06/2025.
//

#include "Cosmos.h"
#include "Astra.h"

Cosmos::Cosmos(const Vector2<int> windows_size) {
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    const auto center = Vector2(DM.w - windows_size.x, DM.h - windows_size.y) / 2;

    const auto right_shift = Vector2(DM.w / 4, 0);
    const auto up_shift = Vector2(0, DM.h / 8);

    const std::vector green{0, 255, 0};
    const std::vector pink{255, 0, 184};

    const auto astra_green = new Astra(
        "Emerald",
        center + right_shift - up_shift,
        Vector2(windows_size.x, windows_size.y),
        green
    );
    const auto astra_pink = new Astra(
        "Magenta",
        center - right_shift + up_shift,
        Vector2(windows_size.x, windows_size.y),
        pink
    );

    astras.push_back(astra_green);
    astras.push_back(astra_pink);

    // Initialize drag state
    is_dragging = false;
    dragged_window = nullptr;
    drag_offset = Vector2(0, 0);
}

Cosmos::~Cosmos() {
    for (const auto astra: astras) {
        delete astra;
    }
}

void Cosmos::draw(const bool debug) const {
    const Astra *astra1 = astras.front();
    const auto a1_renderer = astra1->renderer;
    const Astra *astra2 = astras.back();
    const auto a2_renderer = astra2->renderer;

    astra1->clear_window();
    astra2->clear_window();

    const auto any_astra = astra1; // pour les caractéristiques communes

    std::vector<int> particle_color;
    int i;
    int color_index = 0;

    std::vector<SDL_Point> a1_points_to_draw;
    a1_points_to_draw.reserve(std::max(astra1->particles.size(), astra1->core_particles.size()));
    std::vector<SDL_Point> a2_points_to_draw;
    a2_points_to_draw.reserve(std::max(astra2->particles.size(), astra2->core_particles.size()));


    // Particules libres
    int same_color_particle_count = any_astra->particles.size() / 6;
    int same_color_index = same_color_particle_count;

    for (i = 0; i < any_astra->particles.size(); i++) {
        auto p = astra1->particles[i];
        auto window_pos = astra1->screen_to_window(p->pos);
        a1_points_to_draw.push_back(window_pos.as_SDL_Point());
        p = astra2->particles[i];
        window_pos = astra2->screen_to_window(p->pos);
        a2_points_to_draw.push_back(window_pos.as_SDL_Point());

        if (i == same_color_index || i == any_astra->particles.size() - 1) {
            particle_color = astra1->particle_colors[color_index];
            SDL_SetRenderDrawColor(a1_renderer, particle_color[0], particle_color[1], particle_color[2], 255);
            SDL_SetRenderDrawColor(a2_renderer, particle_color[0], particle_color[1], particle_color[2], 255);

            std::vector<SDL_Point> a1_points_for_a2;
            std::ranges::transform(a1_points_to_draw, std::back_inserter(a1_points_for_a2),
                                   SDL_Point_converter(astra1, astra2));

            SDL_RenderDrawPoints(a1_renderer, a1_points_to_draw.data(), static_cast<int>(a1_points_to_draw.size()));
            SDL_RenderDrawPoints(a2_renderer, a1_points_for_a2.data(), static_cast<int>(a1_points_for_a2.size()));

            particle_color = astra2->particle_colors[color_index];
            SDL_SetRenderDrawColor(a1_renderer, particle_color[0], particle_color[1], particle_color[2], 255);
            SDL_SetRenderDrawColor(a2_renderer, particle_color[0], particle_color[1], particle_color[2], 255);

            std::vector<SDL_Point> a2_points_for_a1;
            std::ranges::transform(a2_points_to_draw, std::back_inserter(a2_points_for_a1),
                                   SDL_Point_converter(astra2, astra1));

            SDL_RenderDrawPoints(a1_renderer, a2_points_for_a1.data(), static_cast<int>(a2_points_for_a1.size()));
            SDL_RenderDrawPoints(a2_renderer, a2_points_to_draw.data(), static_cast<int>(a2_points_to_draw.size()));

            a1_points_to_draw.clear();
            a2_points_to_draw.clear();

            color_index++;
            same_color_index += same_color_particle_count;
        }
    }


    // Particules centrales
    same_color_particle_count = any_astra->core_particles.size() / 6;
    same_color_index = same_color_particle_count;

    for (i = 0; i < any_astra->core_particles.size(); i++) {
        auto p = astra1->core_particles[i];
        a1_points_to_draw.push_back(p->pos.as_SDL_Point());
        p = astra2->core_particles[i];
        a2_points_to_draw.push_back(p->pos.as_SDL_Point());

        if (i == same_color_index || i == any_astra->core_particles.size() - 1) {
            particle_color = astra1->particle_colors[color_index];
            SDL_SetRenderDrawColor(a1_renderer, particle_color[0], particle_color[1], particle_color[2], 255);
            SDL_SetRenderDrawColor(a2_renderer, particle_color[0], particle_color[1], particle_color[2], 255);

            std::vector<SDL_Point> a1_points_for_a2;
            std::ranges::transform(a1_points_to_draw, std::back_inserter(a1_points_for_a2),
                                   SDL_Point_converter(astra1, astra2));

            SDL_RenderDrawPoints(a1_renderer, a1_points_to_draw.data(), static_cast<int>(a1_points_to_draw.size()));
            SDL_RenderDrawPoints(a2_renderer, a1_points_for_a2.data(), static_cast<int>(a1_points_for_a2.size()));

            particle_color = astra2->particle_colors[color_index];
            SDL_SetRenderDrawColor(a1_renderer, particle_color[0], particle_color[1], particle_color[2], 255);
            SDL_SetRenderDrawColor(a2_renderer, particle_color[0], particle_color[1], particle_color[2], 255);

            std::vector<SDL_Point> a2_points_for_a1;
            std::ranges::transform(a2_points_to_draw, std::back_inserter(a2_points_for_a1),
                                   SDL_Point_converter(astra2, astra1));

            SDL_RenderDrawPoints(a1_renderer, a2_points_for_a1.data(), static_cast<int>(a2_points_for_a1.size()));
            SDL_RenderDrawPoints(a2_renderer, a2_points_to_draw.data(), static_cast<int>(a2_points_to_draw.size()));

            a1_points_to_draw.clear();
            a2_points_to_draw.clear();

            color_index++;
            same_color_index += same_color_particle_count;
        }
    }

    if (debug) display_mouse_acc();

    astra1->render_present();
    astra2->render_present();
}

void Cosmos::update(const double dt) const {
    Astra *astra1 = astras.front();
    Astra *astra2 = astras.back();
    astra1->update(dt, 500, astra2);
    astra2->update(dt, 500, astra1);
}

void Cosmos::move_windows(const SDL_Event &e) {
    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                int mouse_x, mouse_y;
                SDL_GetGlobalMouseState(&mouse_x, &mouse_y);

                // Check which window (if any) contains the mouse
                for (auto *astra: astras) {
                    int win_x, win_y, win_w, win_h;
                    SDL_GetWindowPosition(astra->window, &win_x, &win_y);
                    SDL_GetWindowSize(astra->window, &win_w, &win_h);

                    if (mouse_x >= win_x && mouse_x < win_x + win_w &&
                        mouse_y >= win_y && mouse_y < win_y + win_h) {
                        is_dragging = true;
                        dragged_window = astra;
                        drag_offset = Vector2(mouse_x - win_x, mouse_y - win_y);
                        break;
                    }
                }
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT) {
                is_dragging = false;
                dragged_window = nullptr;
            }
            break;

        case SDL_MOUSEMOTION:
            if (is_dragging && dragged_window) {
                int mouse_x, mouse_y;
                SDL_GetGlobalMouseState(&mouse_x, &mouse_y);

                const int new_x = mouse_x - drag_offset.x;
                const int new_y = mouse_y - drag_offset.y;

                SDL_SetWindowPosition(dragged_window->window, new_x, new_y);
            }
            break;

        default:
            break;
    }
}


void Cosmos::display_mouse_acc() const {
    const auto src = astras[0];
    const auto dest = astras[1];

    int x, y;
    SDL_GetGlobalMouseState(&x, &y);

    auto mouse = Particle({static_cast<double>(x), static_cast<double>(y)}, {});
    mouse.spin = 1;

    if (src->is_overlapping_with_other_window(dest)) {
        if (dest->get_star_screen_position().dist2(&mouse.pos) >= ASTRA_RADIUS * ASTRA_RADIUS)
            mouse.update_a_gravity_2(src->get_star_screen_position(), dest->get_star_screen_position(), dest->m,
                                     0.5);
        else
            mouse.update_a_gravity(src->get_star_screen_position(), dest->get_star_screen_position(), src->m);
    } else {
        mouse.update_a_fall(10, src->get_star_screen_position());
    }

    const Vector2<int> pos1 = src->screen_to_window(mouse.pos);
    const Vector2<int> pos2 = dest->screen_to_window(mouse.pos);
    const Vector2<int> a1 = pos1 + mouse.a.as_int();
    const Vector2<int> a2 = pos2 + mouse.a.as_int();

    SDL_RenderDrawLine(src->renderer, pos1.x, pos1.y, a1.x, a1.y);
    SDL_RenderDrawLine(dest->renderer, pos2.x, pos2.y, a2.x, a2.y);
}
