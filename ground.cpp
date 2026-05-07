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

namespace
{
    int clampInt(int value, int minValue, int maxValue)
    {
        if (value < minValue)
        {
            return minValue;
        }
        if (value > maxValue)
        {
            return maxValue;
        }
        return value;
    }
}

std::vector<ground_Class> ground_generator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex)
{
    int screenW = currentScreenWidth;
    int screenH = currentScreenHeight;
    getScreenSize(renderer, screenW, screenH);

    std::random_device rd;
    std::mt19937 gen(rd());

    const int minY = clampInt((int)(screenH * 0.40f), 160, screenH - 260);
    const int maxY = clampInt((int)(screenH * 0.72f), minY + 50, screenH - 120);
    const int minW = clampInt((int)(screenW * 0.60f), 600, screenW + 200);
    const int maxW = clampInt((int)(screenW * 1.25f), minW, screenW + 900);
    const int maxH = clampInt((int)(screenH * 0.32f), 120, screenH / 2);

    std::uniform_int_distribution<> disty(minY, maxY);
    std::uniform_int_distribution<> distw(minW, maxW);
    std::uniform_int_distribution<> disth(50, maxH);

    int x = 0;
    int y = disty(gen);
    int w = distw(gen);
    int h = disth(gen);

    std::vector<ground_Class> ground;

    ground.emplace_back(
        x, y, w, h,
       groundTex,ugTex
    );
    ground[0].first_ground=true;

    return ground;
}


void groundRandomgenerator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex,std::vector<ground_Class> &prevGround,std::vector<trapSpike>& trap)
{   
    if (prevGround.empty())
    {
        return;
    }

    int screenW = currentScreenWidth;
    int screenH = currentScreenHeight;
    getScreenSize(renderer, screenW, screenH);

    bool check=false;
    int x,y,w,h;
    std::random_device rd;
    std::mt19937 gen(rd());

    const int minY = clampInt((int)(screenH * 0.16f), 80, screenH - 260);
    const int maxY = clampInt((int)(screenH * 0.74f), minY + 50, screenH - 120);
    const int minGap = clampInt((int)(screenW * 0.06f), 80, 180);
    const int maxGap = clampInt((int)(screenW * 0.10f), minGap, 260);
    const int minW = clampInt((int)(screenW * 0.16f), 280, screenW);
    const int maxW = clampInt((int)(screenW * 1.25f), minW, screenW + 900);
    const int maxH = clampInt((int)(screenH * 0.32f), 120, screenH / 2);

    std::uniform_int_distribution<> distx(prevGround.back().x + prevGround.back().w + minGap,
                                          prevGround.back().x + prevGround.back().w + maxGap);
    std::uniform_int_distribution<> distw(minW, maxW);
    std::uniform_int_distribution<> disth(50, maxH);
    

    while(!check)
    {
        const int nextMinY = clampInt(prevGround.back().y - (int)(screenH * 0.08f), minY, maxY);
        const int nextMaxY = clampInt(prevGround.back().y + (int)(screenH * 0.48f), nextMinY, maxY);
        std::uniform_int_distribution<> disty(nextMinY, nextMaxY);
        x = distx(gen);
        y = disty(gen);
        w = distw(gen);
        h = disth(gen);
        if(y <= maxY && y >= minY)
        {
            check=true;
        }
    }
    int settrap = 1;
    prevGround.emplace_back(x, y, w, h, groundTex, ugTex);
    int currentnumberindex = (int)prevGround.size() - 1;

    const int smallPlatformLimit = (int)(screenW * 0.35f);
    const int mediumPlatformLimit = (int)(screenW * 0.86f);

    if (prevGround[currentnumberindex].w < smallPlatformLimit)
    {
        prevGround[currentnumberindex].small = true;
        prevGround[currentnumberindex].medium = false;
        prevGround[currentnumberindex].big = false;
        settrap = 1;
    }
    else if (prevGround[currentnumberindex].w < mediumPlatformLimit)
    {
        prevGround[currentnumberindex].small = false;
        prevGround[currentnumberindex].medium = true;
        prevGround[currentnumberindex].big = false;
        settrap = 4;
    }
    else
    {
        prevGround[currentnumberindex].small = false;
        prevGround[currentnumberindex].medium = false;
        prevGround[currentnumberindex].big = true;
        settrap = 6;
    }

    for (int j = 0; j < settrap; j++)
    {
        trapTriangle(renderer, trap, prevGround, currentnumberindex, settrap);
    }
}




// void groundRandomgenerator(SDL_Renderer* renderer,
//                            SDL_Texture* groundTex,
//                            SDL_Texture* ugTex,
//                            std::vector<ground_Class> &prevGround,
//                            std::vector<trapSpike>& trap)
// {   
//     bool check = false;
//     int x, y, w, h;

//     std::random_device rd;
//     std::mt19937 gen(rd());

//     std::uniform_int_distribution<> distx(
//         prevGround.back().x + prevGround.back().w + 120,
//         prevGround.back().x + prevGround.back().w + 300
//     );

//     std::uniform_int_distribution<> distw(300, 2450);
//     std::uniform_int_distribution<> disth(50, 400);

//     for (int i = 0; i < prevGround.size(); i++)
//     {
//         trapTriangle(renderer, trap, prevGround, i);
//     }

//     while (!check)
//     {
//         std::uniform_int_distribution<> disty(
//             prevGround.back().y - 100,
//             prevGround.back().y + 600
//         );

//         x = distx(gen);
//         y = disty(gen);
//         w = distw(gen);
//         h = disth(gen);

//         if (y < 800 && y > 100)
//         {
//             check = true;
//         }
//     }

//     // ✅ Construct directly inside the vector
//     prevGround.emplace_back(x, y, w, h, groundTex, ugTex);
// }

void freeMemory(std::vector<ground_Class>& ground)
{
    for (int i = 0; i < (int)ground.size(); )
    {
        // ✅ Remove platforms that are fully off-screen to the LEFT
        if (ground[i].x + ground[i].w < 0)
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
