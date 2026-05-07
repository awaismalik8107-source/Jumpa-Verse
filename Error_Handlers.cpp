#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include<iostream>
#include<SDL2/SDL.h>
#include<vector>
#include<string.h>
#include<vector>
#include<algorithm>//For basic algorithim like sorting 
#include"object.h"
#include"functions.h"
#include<SDL2/SDL_image.h>



bool init(SDL_Window*& Window,SDL_Renderer *&renderer);//Check 
void close(SDL_Window* window,SDL_Renderer* renderer);

int currentScreenWidth = GAME_WIDTH;
int currentScreenHeight = GAME_HEIGHT;

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


    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
    {
        currentScreenWidth = displayMode.w;
        currentScreenHeight = displayMode.h;
    }

    Window = SDL_CreateWindow("Jumpa Verse",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              currentScreenWidth,
                              currentScreenHeight,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP);

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

    SDL_SetWindowMinimumSize(Window, 960, 540);

    return true;
}

void getScreenSize(SDL_Renderer* renderer, int& screenW, int& screenH)
{
    screenW = currentScreenWidth;
    screenH = currentScreenHeight;

    if (!renderer)
    {
        return;
    }

    if (SDL_GetRendererOutputSize(renderer, &screenW, &screenH) == 0)
    {
        currentScreenWidth = screenW;
        currentScreenHeight = screenH;
    }
}

bool handleWindowControlEvent(SDL_Renderer* renderer, const SDL_Event& event)
{
    if (!renderer || event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
    {
        return false;
    }

    int screenW = currentScreenWidth;
    int screenH = currentScreenHeight;
    getScreenSize(renderer, screenW, screenH);

    const int buttonSize = 46;
    const int gap = 12;
    const int top = 18;
    SDL_Rect windowButton = {screenW - 18 - buttonSize, top, buttonSize, buttonSize};
    SDL_Rect minimizeButton = {windowButton.x - gap - buttonSize, top, buttonSize, buttonSize};

    const int mx = event.button.x;
    const int my = event.button.y;
    SDL_Window* window = SDL_RenderGetWindow(renderer);
    if (!window)
    {
        return false;
    }

    if (mx >= minimizeButton.x && mx <= minimizeButton.x + minimizeButton.w &&
        my >= minimizeButton.y && my <= minimizeButton.y + minimizeButton.h)
    {
        SDL_MinimizeWindow(window);
        return true;
    }

    if (mx >= windowButton.x && mx <= windowButton.x + windowButton.w &&
        my >= windowButton.y && my <= windowButton.y + windowButton.h)
    {
        const Uint32 flags = SDL_GetWindowFlags(window);
        if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
        {
            SDL_SetWindowFullscreen(window, 0);
            SDL_SetWindowSize(window, GAME_WIDTH, GAME_HEIGHT);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
        else
        {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        return true;
    }

    return false;
}

void renderWindowControls(SDL_Renderer* renderer)
{
    if (!renderer)
    {
        return;
    }

    int screenW = currentScreenWidth;
    int screenH = currentScreenHeight;
    getScreenSize(renderer, screenW, screenH);

    const int buttonSize = 46;
    const int gap = 12;
    const int top = 18;
    SDL_Rect windowButton = {screenW - 18 - buttonSize, top, buttonSize, buttonSize};
    SDL_Rect minimizeButton = {windowButton.x - gap - buttonSize, top, buttonSize, buttonSize};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 185);
    SDL_RenderFillRect(renderer, &minimizeButton);
    SDL_RenderFillRect(renderer, &windowButton);

    SDL_SetRenderDrawColor(renderer, 255, 127, 0, 255);
    SDL_RenderDrawRect(renderer, &minimizeButton);
    SDL_RenderDrawRect(renderer, &windowButton);

    SDL_RenderDrawLine(renderer,
                       minimizeButton.x + 12,
                       minimizeButton.y + buttonSize / 2 + 9,
                       minimizeButton.x + buttonSize - 12,
                       minimizeButton.y + buttonSize / 2 + 9);

    SDL_Rect windowIcon = {
        windowButton.x + 13,
        windowButton.y + 13,
        buttonSize - 26,
        buttonSize - 26
    };
    SDL_RenderDrawRect(renderer, &windowIcon);
}

void close(SDL_Window* window,SDL_Renderer* renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
     IMG_Quit();
    SDL_Quit();
   
} 
