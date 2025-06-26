//
// Created by Hippolyte Cosserat on 19/06/2025.
//

#include <cmath>

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
}

Cosmos::~Cosmos() {
    for (const auto astra: astras) {
        delete astra;
    }
}

void Cosmos::draw() const {
    const Astra *astra1 = astras.front();
    const Astra *astra2 = astras.back();
    astra1->draw(astra2);
    astra2->draw(astra1);
}

void Cosmos::update(const double dt) const {
    Astra *astra1 = astras.front();
    Astra *astra2 = astras.back();
    astra1->update(dt, 500, astra2);
    astra2->update(dt, 500, astra1);
}
