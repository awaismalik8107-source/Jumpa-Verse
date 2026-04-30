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



const int MAINWIDTH=1980;
const int MAINHEIGHT=1260;

bool init(SDL_Window*& Window,SDL_Renderer *&renderer);//Check 
void close(SDL_Window* window,SDL_Renderer* renderer);

bool init(SDL_Window*& Window,SDL_Renderer *&renderer)
{
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        std::cerr<<"SDL couldnt be initialized! \t\t\t Error Type:\t"<<SDL_GetError()<<std::endl;
        return false;
    }
    if(TTF_Init()<0)
    {
        std::cout<<"TTF NOT FOUND"<<std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG))
{
    std::cerr << "SDL_image could not initialize! Error: " << IMG_GetError() << std::endl;
    return false;
}


    Window = SDL_CreateWindow("Jumpa Verse",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,MAINWIDTH,MAINHEIGHT,SDL_WINDOW_SHOWN);

    if(!Window)
    {
        std::cerr<<"SDL WINDOW COULDNT START \t\t\tERROR TYPE:\t"<<SDL_GetError()<<std::endl;
        return false;
    }
    renderer=SDL_CreateRenderer(Window,-1,SDL_RENDERER_ACCELERATED);

    if(!renderer)
    {
        std::cerr<<"RENDERER COULDNT LOAD!\t\t\tSDL ERROR:\t"<<SDL_GetError()<<std::endl;
        return false;
    }
    return true;
}

void close(SDL_Window* window,SDL_Renderer* renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
     IMG_Quit();
    SDL_Quit();
   
} 
