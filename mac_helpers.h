#pragma once
#include <SDL.h>

#ifdef __APPLE__
void ApplyMacRoundedCorners(SDL_Window *sdlWindow, int radius);
extern "C" void SetupMenu();
#endif
