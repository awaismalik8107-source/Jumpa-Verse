#pragma once
#include <SDL2/SDL.h>
#include <vector>

class ground_Class;

bool init(SDL_Window*& Window, SDL_Renderer*& renderer);
void close(SDL_Window* window, SDL_Renderer* renderer);

std::vector<ground_Class> ground_generator(SDL_Renderer* renderer);