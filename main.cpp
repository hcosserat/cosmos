#include <iostream>

#include "Cosmos.h"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize >< ! Error : " << SDL_GetError() << std::endl;
        return 1;
    }

    constexpr int fps = 30;
    constexpr double dt = 1.0 / fps;
    const auto cosmos = new Cosmos(Vector2(600, 600));
    bool quit = false;

    SDL_Event e;

    bool debug = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT
                || (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d) {
                debug = !debug;
            }
        }

        cosmos->update(dt);
        cosmos->draw();
        SDL_Delay(static_cast<Uint32>(1000 * dt));
    }

    delete cosmos;
    SDL_Quit();

    return 0;
}
