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


void trapTriangle(SDL_Renderer* renderer,std::vector<trapSpike>& trap,std::vector<ground_Class>& ground,int i)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    while(1)
    {
    std::uniform_int_distribution<> distp(ground[i].x,ground[i].x+ground[i].w-100);
    std::uniform_int_distribution<> chances(0, 100);
    if(trap.size()>3)
    {
    int indexc=trap.size()-1;
    if(distp(gen)<trap[indexc-1].x)
    break;
    }
    
    if (chances(gen) < 5 && ground[i].trapCount<4 )
    {
        ground[i].trapCount++;
        trap.emplace_back(distp(gen), ground[i].y, ground[i].w, ground[i].h);
        break;
    }
}


}


void trapTriangleInitalization(SDL_Renderer* renderer,std::vector<trapSpike>& trap,std::vector<ground_Class>& ground,int i=1)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distp(ground[i].x,ground[i].x+ground[i].w-50);
    std::uniform_int_distribution<> chances(0, 100);

    if (chances(gen) < 30)
    {
        
        trap.emplace_back(distp(gen), ground[i].y, ground[i].w, ground[i].h);
    }
}

void trapFreeMemory(std::vector<trapSpike>&trap)
{
    if(trap.size()>20)
    {
        trap.erase(trap.begin());
    }
}