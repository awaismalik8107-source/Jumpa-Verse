#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <string>

class hitBox
{
public:
    int x, y, w, h;

    hitBox(int x, int y, int w, int h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
};

class ground_Class : public hitBox
{
public:
    int divw = 0;

    std::vector<SDL_Rect> ground;
    std::vector<std::vector<SDL_Rect>> groundUnder;

    SDL_Texture* groundTexture = nullptr;  // shared (NOT owned)
    SDL_Texture* ugTexture = nullptr;      // shared (NOT owned)

    int perboxwidth = 50;
    int perboxheight = 50;

public:
    // Default constructor
    ground_Class() : hitBox(0, 0, 0, 0) {}

    // Main constructor (USES SHARED TEXTURES)
    ground_Class(int ax, int ay, int aw, int ah,
                 SDL_Texture* groundTex,
                 SDL_Texture* ugTex)
        
        : hitBox(ax, ay, aw, ah),
          groundTexture(groundTex),
          ugTexture(ugTex)
    {
        rebuild();
    }

    // Rebuild geometry only (NO SDL loading here)
    void rebuild()
    {
        divw = w / perboxwidth;
        int rows = h / perboxheight;

        ground.clear();
        groundUnder.clear();

        ground.resize(divw);
        groundUnder.resize(divw);

        for (int i = 0; i < divw; i++)
        {
            groundUnder[i].resize(rows);
        }

        for (int i = 0; i < divw; i++)
        {
            ground[i] = { x + i * perboxwidth, y, perboxwidth, perboxheight };

            for (int j = 0; j < rows; j++)
            {
                groundUnder[i][j] =
                {
                    x + i * perboxwidth,
                    y + (j + 1) * perboxheight,
                    perboxwidth,
                    perboxheight
                };
            }
        }
    }

    void seterForElements(int newX, int newY, int newW, int newH)
    {
        x = newX;
        y = newY;
        w = newW;
        h = newH;

        rebuild();
    }

    void render(SDL_Renderer* renderer)
    {
        if (!groundTexture || !ugTexture)
        {
            std::cerr << "Cannot render: texture is null!\n";
            return;
        }

        for (int i = 0; i < divw; i++)
        {
            SDL_RenderCopy(renderer, groundTexture, nullptr, &ground[i]);

            for (int j = 0; j < (int)groundUnder[i].size(); j++)
            {
                SDL_RenderCopy(renderer, ugTexture, nullptr, &groundUnder[i][j]);
            }
        }
    }

    
   
};