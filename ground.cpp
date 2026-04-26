#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include<iostream>
#include<SDL2/SDL.h>
#include <iterator>
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
/*


*/






    ground_Class groundRandomgenerator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex,std::vector<ground_Class> &prevGround,std::vector<trapSpike>& trap)
{   
 bool check=false;
 int x,y,w,h;
     std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distx(prevGround.back().x+ prevGround.back().w+80,prevGround.back().x +prevGround.back().w+ 120);
    
    // std::uniform_int_distribution<> distx(500, 900);
    // std::uniform_int_distribution<> disty(500, 900);
    std::uniform_int_distribution<> distw(300, 2450);
    std::uniform_int_distribution<> disth(50, 400);

    for(int i=0;i<prevGround.size();i++)
    {
        trapTriangle(renderer,trap,prevGround,i);
    }
    while(!check)
    {
   std::uniform_int_distribution<> disty(prevGround.back().y-30, prevGround.back().y+60);
    x = distx(gen);
    y = disty(gen);
    w = distw(gen);
    h = disth(gen);
    if(y<800 &&y>100)
    {
        check=true;
    }
    }
    
    ground_Class temp(x,y,w,h,groundTex,ugTex);
    

    return temp;
}

void freeMemory(std::vector<ground_Class>& ground)
{
    for (int i = 0; i < ground.size(); )
    {
        if (ground[i].x >= 2400)
            ground.erase(ground.begin() + i);
        else
            i++;
    }
}

void dualPath(std::vector<ground_Class>& ground,SDL_Texture* groundTex,SDL_Texture* ugTex)
{

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution <> chances (0,1000);

    if(chances(gen)<=1)
    {

    std::uniform_int_distribution<> distx(ground.back().x+ ground.back().w+80,ground.back().x +ground.back().w+ 120);
    std::uniform_int_distribution<> disty(ground.back().y+150,ground.back().y+250);
    // std::uniform_int_distribution<> distx(500, 900);
    // std::uniform_int_distribution<> disty(500, 900);
    std::uniform_int_distribution<> distw(300, 1400);
    std::uniform_int_distribution<> disth(50, 100);

    }
}