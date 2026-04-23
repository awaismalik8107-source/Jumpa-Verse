#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include<iostream>
#include<SDL2/SDL.h>
#include<vector>
#include<string.h>
#include<vector>
#include<algorithm>//For basic algorithim like sorting 
#include"object.h"
#include<SDL2/SDL_image.h>
#include<random>
#include "functions.h"

std::vector<ground_Class> ground_generator(SDL_Renderer* renderer)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> disty(500, 900);
    std::uniform_int_distribution<> distw(300, 950);
    std::uniform_int_distribution<> disth(50, 400);

    int x = 0;
    int y = disty(gen);
    int w = distw(gen);
    int h = disth(gen);

    std::vector<ground_Class> ground;

    ground.emplace_back(
        x, y, w, h,
        renderer,
        "Images_textures/ground.jpeg",
        "Images_textures/underGround.jpeg"
    );

    return ground;
}