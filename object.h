#pragma once 
#include<SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include<iostream>
#include<string>
#include<vector>
#include<SDL2/SDL_image.h>
/*

vector of subg --> than later we create a distance

*/


 class ground_Class
 {
    private:
    int x;
    int y;
    int w;
    int h;
    int divw=0;//Divide width into 20px parts and get widths
     std::vector <SDL_Rect> ground ;
     std::vector <std::vector<SDL_Rect>> groundUnder;
     SDL_Texture* groundTexture;
     SDL_Texture* ugTexture;
     
     int ux,uy,uw,uh;
     int perboxwidth=50;
     int perboxheight=50;
     int perboxX=50;
     int perboxY;
     



    public:
        
        ground_Class() : x(0), y(0), w(0), h(0), divw(0) {}
        ground_Class(int ax,int ay,int aw,int ah,SDL_Renderer*renderer,const std::string &texturePath,const std::string &ugT)
        {
            x=ax;
            y=ay;
            w=aw;
            h=ah;
            divw = w / perboxwidth;
            int rows = h / perboxheight;

            ground.resize(divw);
           groundUnder.resize(divw);
            for (int i = 0; i < divw; i++)
            {
                groundUnder[i].resize(rows);
            }

            SDL_Texture* texture = loadTexture(renderer, texturePath);
            ugTexture=loadTexture(renderer,ugT);

             groundTexture=texture;

            for (int i=0;i<divw;i++)
            {
                ground[i]={x + i*50,y,50,50};
               

            for(int j = 0; j < groundUnder[i].size(); j++)
                groundUnder[i][j] = { x + i * 50, y + (j+1) * 50, 50, 50 };

            }
        }

  SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path)
{
    SDL_Surface* surface = IMG_Load(path.c_str());

    if (!surface)
    {
        std::cerr << "IMG_Load FAILED: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        std::cerr << "Texture Create FAILED: " << SDL_GetError() << std::endl;
    }

    return texture;
}

    void render(SDL_Renderer* renderer)
{
    if (!groundTexture || !ugTexture) {
        std::cerr << "Cannot render: texture is null!" << std::endl;
        return;
    }

    for (int i = 0; i < divw; i++)
    {
        SDL_RenderCopy(renderer, groundTexture, NULL, &ground[i]);
        for (int j = 0; j < (int)groundUnder[i].size(); j++)
            SDL_RenderCopy(renderer, ugTexture, NULL, &groundUnder[i][j]);
    }
}
     ~ground_Class()
    {
        if (groundTexture)
    {
        SDL_DestroyTexture(groundTexture);
        SDL_DestroyTexture(ugTexture);
    }
    }

    
 };


   