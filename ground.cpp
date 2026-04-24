#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include<iostream>
#include<SDL2/SDL.h>
#include<vector>
#include<string.h>
#include<algorithm>//For basic algorithim like sorting 
#include"object.h"
#include<SDL2/SDL_image.h>
#include<random>
#include "functions.h"

std::vector<ground_Class> ground_generator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> disty(500, 900);
    std::uniform_int_distribution<> distw(300, 2450);
    std::uniform_int_distribution<> disth(50, 400);

    int x = 0;
    int y = disty(gen);
    int w = distw(gen);
    int h = disth(gen);

    std::vector<ground_Class> ground;

    ground.emplace_back(
        x, y, w, h,
       groundTex,ugTex
    );

    return ground;
}

    ground_Class groundRandomgenerator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex,std::vector<ground_Class> &prevGround)
{   

     std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distx(prevGround.back().x+ prevGround.back().w+80,prevGround.back().x +prevGround.back().w+ 120);
    std::uniform_int_distribution<> disty(prevGround.back().y-30, prevGround.back().y+60);
    // std::uniform_int_distribution<> distx(500, 900);
    // std::uniform_int_distribution<> disty(500, 900);
    std::uniform_int_distribution<> distw(300, 2450);
    std::uniform_int_distribution<> disth(50, 400);
    int x = distx(gen);
    int y = disty(gen);
    int w = distw(gen);
    int h = disth(gen);

    ground_Class temp(x,y,w,h,groundTex,ugTex);
        
        return temp;
}