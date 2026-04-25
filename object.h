#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL2_gfxPrimitives.h>//to import trigons

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
    int trapCount=0;

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


class trapSpike :public hitBox
{
    public:
    SDL_Point p1;
    SDL_Point p2;
    SDL_Point p3;

    trapSpike():hitBox(0,0,0,0)
    {

    }

    trapSpike(int x,int y,int w,int h):hitBox(x,y,w,h)
    {

        p1={x,y};
        p2={x+50,y};
        p3={x+25,y-50};

        
        /*
        Trigon is present in #include <SDL2/SDL2_gfxPrimitives.h>
        and is refered in the tutorial_image 
        Trigon is a 3 dimension
        Parameters:
            renderer	The renderer to draw on.
            x1	X coordinate of the first point of the trigon.
            y1	Y coordinate of the first point of the trigon.
            x2	X coordinate of the second point of the trigon.
            y2	Y coordinate of the second point of the trigon.
            x3	X coordinate of the third point of the trigon.
            y3	Y coordinate of the third point of the trigon.
            r	The red value of the trigon to draw.
            g	The green value of the trigon to draw.
            b	The blue value of the trigon to draw.
            a	The alpha value of the trigon to draw.
            

            all the information is present in https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/_s_d_l2__gfx_primitives_8h.html#a152662f6985587d137837086aaa95311
        */
    //    filledTrigonRGBA(renderer,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,255,255,255,255);

    }

    void setterNew(int x)
    {
        this->x=x;
        p1={x,y};
        p2={x+50,y};
        p3={x+25,y-50};
    }

    void render(SDL_Renderer* renderer, float cameraOffset=0)
{
    
    filledTrigonRGBA(renderer,
        p1.x + cameraOffset, p1.y,
        p2.x + cameraOffset, p2.y,
        p3.x + cameraOffset, p3.y,
        255,255,255,255);

    trigonRGBA(renderer,
        p1.x + cameraOffset, p1.y,
        p2.x + cameraOffset, p2.y,
        p3.x + cameraOffset, p3.y,
        0,0,0,255);
}

      void updatePosition(int x, int y)
    {
        // Useful for camera movement or animation
        p1 = {x, y};
        p2 = {x + w, y};
        p3 = {x + w/2, y - h};
    }



};