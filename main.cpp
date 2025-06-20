#include <iostream>

#include "Cosmos.h"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    constexpr int fps = 60;
    constexpr double dt = 1.0 / fps;
    const auto cosmos = new Cosmos(2);
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
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
