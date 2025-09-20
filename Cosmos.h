//
// Created by Hippolyte Cosserat on 19/06/2025.
//

#ifndef COSMOS_H
#define COSMOS_H

#include <vector>
#include "Astra.h"
#include "Vector2.h"

class Cosmos {
    std::vector<Astra *> astras;
    bool is_dragging;
    Astra *dragged_window;
    Vector2<int> drag_offset;

public:
    explicit Cosmos(Vector2<int> windows_size = Vector2(500, 500));

    ~Cosmos();

    void draw(bool debug = false) const;

    void update(double dt) const;

    void move_windows(const SDL_Event &e);

    void display_mouse_acc() const;
};

#endif //COSMOS_H
