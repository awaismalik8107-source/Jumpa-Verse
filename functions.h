#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <SDL2/SDL_ttf.h>
extern bool exitb;
class ground_Class;
class trapSpike;
bool init(SDL_Window*& Window, SDL_Renderer*& renderer);
void close(SDL_Window* window, SDL_Renderer* renderer);

std::vector<ground_Class> ground_generator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex);


ground_Class groundRandomgenerator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex,std::vector<ground_Class> &prevGround,std::vector<trapSpike>& trap);

// std::vector<ground_Class> cameraMovmentObj(std::vector<ground_Class> ground,float cameraOffset);

// std::vector<ground_Class> freeMemory(std::vector<ground_Class> ground);
void freeMemory(std::vector<ground_Class>& ground);
void cameraMovmentObj(std::vector<ground_Class>& ground, float cameraOffset,std::vector<trapSpike>&trap);


void trapTriangle(SDL_Renderer* renderer,std::vector<trapSpike>& trap,std::vector<ground_Class>& ground,int i);
void renderTrap(SDL_Renderer* renderer,
                std::vector<trapSpike>& trap,
                float cameraOffset);


void trapFreeMemory(std::vector<trapSpike>&trap);

bool menuScreen(SDL_Renderer* renderer, TTF_Font* font);