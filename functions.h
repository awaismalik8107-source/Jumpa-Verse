#pragma once
#include <SDL2/SDL.h>
#include <vector>

class ground_Class;

bool init(SDL_Window*& Window, SDL_Renderer*& renderer);
void close(SDL_Window* window, SDL_Renderer* renderer);

std::vector<ground_Class> ground_generator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex);


ground_Class groundRandomgenerator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex,std::vector<ground_Class> &prevGround);

std::vector<ground_Class> cameraMovmentObj(std::vector<ground_Class> ground,float cameraOffset);