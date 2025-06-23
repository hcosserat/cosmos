//
// Created by Hippolyte Cosserat on 19/06/2025.
//

#include <cmath>

#include "Cosmos.h"
#include "Astra.h"

Cosmos::Cosmos(const Vector2<int> windows_size) {
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    const int centerx = DM.w / 2 - windows_size.x / 2;
    const int centery = DM.h / 2 - windows_size.y / 2;

    const double dist_from_center = DM.h / 2.0;

    for (int i = 0; i < 2; i++) {
        constexpr double angle_shift = 0.8;
        const double pos_angle = i * M_PI + angle_shift;
        const int x = centerx + dist_from_center * cos(pos_angle);
        const int y = centery + dist_from_center * sin(pos_angle);

        std::vector green{0, 255, 0};
        std::vector pink{255, 4, 191};

        const auto astra = new Astra(
            std::to_string(i).c_str(),
            Vector2(x, y),
            Vector2(windows_size.x, windows_size.y),
            i == 0 ? green : pink
        );

        astra->cosmos = this;
        astras.push_back(astra);
    }
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
