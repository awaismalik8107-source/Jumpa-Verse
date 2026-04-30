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

SDL_Color mainMenuColor::background   = {30, 30, 30, 255};
SDL_Color mainMenuColor::buttonNormal = {70, 130, 180, 255};
SDL_Color mainMenuColor::buttonHover  = {100, 160, 210, 255};
SDL_Color mainMenuColor::buttonActive = {40, 90, 140, 255};
SDL_Color mainMenuColor::text         = {255, 255, 255, 255};
//Run 
bool menuScreen(SDL_Renderer* renderer, TTF_Font* font)
{
    optionBoxes playButton(740, 430, 500, 150);
    /*
    1980/2=990-widthof the box
    1260/2=630-200
    */
    playButton.box_Initializer(renderer, font, "Play");
    SDL_Point start={0,1000};
    SDL_Point end={1980,1000};

    bool running = true;
    SDL_Event event;
    int mousex,mousey;
    bool hover=false;

    

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return false;

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mx = event.button.x;
                int my = event.button.y;

                // Check click inside button
                if (mx >= playButton.x && mx <= playButton.x + playButton.w &&
                    my >= playButton.y && my <= playButton.y + playButton.h)
                {
                    return true; 
                }

                if (mx >= playButton.boxExit.x && mx <= playButton.boxExit.x + playButton.boxExit.w &&
                    my >= playButton.boxExit.y && my <= playButton.boxExit.y + playButton.boxExit.h)
                    {
                    exitb=true;
                    return true;
                    }
            }
        }
        // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white
        // SDL_RenderDrawLine(renderer,start.x,start.y,end.x,end.y);

        Uint32 button = SDL_GetMouseState(&mousex,&mousey);
       if(mousex>=playButton.x && mousex<=playButton.w+playButton.x && mousey>=playButton.y && mousey<=playButton.y+playButton.h)
        {
            std::cout<<mousex<<" , "<<mousey<<std::endl;
            // playButton.state=true;
            
            playButton.stateHolder(renderer, font, "Play",true,false);
        }
        // else{
        //     // playButton.state=false;
        //     playButton.stateHolder(renderer, font, "Play",false,false);
        // }

        else if(mousex>=playButton.boxExit.x && mousex<=playButton.boxExit.x + playButton.boxExit.w && mousey>=playButton.boxExit.y && mousey<=playButton.boxExit.y+ playButton.boxExit.h)
        {
           playButton.stateHolder(renderer, font, "Play",false,true); 
        }
        else
        {
            playButton.stateHolder(renderer, font, "Play",false,false); 
        }

             // Clear screen
        SDL_SetRenderDrawColor(renderer,
            mainMenuColor::background.r,
            mainMenuColor::background.g,
            mainMenuColor::background.b,
            mainMenuColor::background.a);

        SDL_RenderClear(renderer);
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
        // Render button
        playButton.render(renderer);

        SDL_RenderPresent(renderer);
    }

    return false;
}