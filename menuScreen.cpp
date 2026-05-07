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
#include<SDL2/SDL2_gfxPrimitives.h>
#include<random>
#include "functions.h"

SDL_Color mainMenuColor::background   = {30, 30, 30, 255};
SDL_Color mainMenuColor::buttonNormal = {70, 130, 180, 255};
SDL_Color mainMenuColor::buttonHover  = {100, 160, 210, 255};
SDL_Color mainMenuColor::buttonActive = {40, 90, 140, 255};
SDL_Color mainMenuColor::text         = {255, 255, 255, 255};

namespace
{
    bool openLoginForPlay(SDL_Renderer* renderer, TTF_Font* font)
    {
        LoginScreenAction action = runLoginScreen(renderer, font);
        if (action == LoginScreenAction::Quit)
        {
            exitb = true;
            return true;
        }

        return action == LoginScreenAction::LoginSuccess;
    }
}

//Run 
bool menuScreen(SDL_Renderer* renderer, TTF_Font* font)
{
    optionBoxes playButton(740, 430, 500, 150);
    /*
    1980/2=990-widthof the box
    1260/2=630-200
    */
    playButton.box_Initializer(renderer, font, "Play");
    SDL_Texture* titleTexture = nullptr;
    if (font)
    {
        SDL_Surface* titleSurface = TTF_RenderText_Blended(font, "Jumpa Verse", SDL_Color{255, 127, 0, 255});
        if (titleSurface)
        {
            titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
            SDL_FreeSurface(titleSurface);
        }
    }
    SDL_Point start={0,1100};
    SDL_Point end={1980,1100};

    bool running = true;
    SDL_Event event;
    int mousex,mousey;
    int selectedButton = 0;
    bool playSelected = true;
    bool scoresSelected = false;
    bool exitSelected = false;
    Uint32 lastFrameTime = SDL_GetTicks();
    float cubeVelocity = 0.0f;
    float cubeY = 1000.0f - 45.0f;
    float trapOffset = 0.0f;

    playButton.stateHolder(renderer, font, "Play", playSelected, scoresSelected, exitSelected);

    

    while (running)
    {
        Uint32 currentFrameTime = SDL_GetTicks();
        float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentFrameTime;
        if (deltaTime > 0.05f)
            deltaTime = 0.05f;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                if (titleTexture)
                    SDL_DestroyTexture(titleTexture);
                return false;
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN)
                {
                    if (event.key.keysym.sym == SDLK_UP)
                        selectedButton = (selectedButton + 2) % 3;
                    else
                        selectedButton = (selectedButton + 1) % 3;

                    playSelected = selectedButton == 0;
                    scoresSelected = selectedButton == 1;
                    exitSelected = selectedButton == 2;
                }
                else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE)
                {
                    if (selectedButton == 0)
                    {
                        if (openLoginForPlay(renderer, font))
                        {
                            if (titleTexture)
                                SDL_DestroyTexture(titleTexture);
                            return true;
                        }
                    }

                    if (selectedButton == 1)
                    {
                        runScoreMenu(renderer, font);
                        if (exitb)
                        {
                            if (titleTexture)
                                SDL_DestroyTexture(titleTexture);
                            return true;
                        }
                    }

                    if (selectedButton == 2)
                    {
                        exitb = true;
                        if (titleTexture)
                            SDL_DestroyTexture(titleTexture);
                        return true;
                    }
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mx = event.button.x;
                int my = event.button.y;

                // Check click inside button
                if (mx >= playButton.x && mx <= playButton.x + playButton.w &&
                    my >= playButton.y && my <= playButton.y + playButton.h)
                {
                    if (openLoginForPlay(renderer, font))
                    {
                        if (titleTexture)
                            SDL_DestroyTexture(titleTexture);
                        return true; 
                    }
                }

                if (mx >= playButton.boxScores.x && mx <= playButton.boxScores.x + playButton.boxScores.w &&
                    my >= playButton.boxScores.y && my <= playButton.boxScores.y + playButton.boxScores.h)
                {
                    selectedButton = 1;
                    playSelected = false;
                    scoresSelected = true;
                    exitSelected = false;
                    runScoreMenu(renderer, font);
                    if (exitb)
                    {
                        if (titleTexture)
                            SDL_DestroyTexture(titleTexture);
                        return true;
                    }
                }

                if (mx >= playButton.boxExit.x && mx <= playButton.boxExit.x + playButton.boxExit.w &&
                    my >= playButton.boxExit.y && my <= playButton.boxExit.y + playButton.boxExit.h)
                    {
                    exitb=true;
                    if (titleTexture)
                        SDL_DestroyTexture(titleTexture);
                    return true;
                    }
            }
        }
        // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white
        // SDL_RenderDrawLine(renderer,start.x,start.y,end.x,end.y);

        Uint32 button = SDL_GetMouseState(&mousex,&mousey);
        (void)button;
       if(mousex>=playButton.x && mousex<=playButton.w+playButton.x && mousey>=playButton.y && mousey<=playButton.y+playButton.h)
        {
            selectedButton = 0;
            playSelected = true;
            scoresSelected = false;
            exitSelected = false;
        }
        // else{
        //     // playButton.state=false;
        //     playButton.stateHolder(renderer, font, "Play",false,false);
        // }

        else if(mousex>=playButton.boxScores.x && mousex<=playButton.boxScores.x + playButton.boxScores.w && mousey>=playButton.boxScores.y && mousey<=playButton.boxScores.y+ playButton.boxScores.h)
        {
            selectedButton = 1;
            playSelected = false;
            scoresSelected = true;
            exitSelected = false;
        }

        else if(mousex>=playButton.boxExit.x && mousex<=playButton.boxExit.x + playButton.boxExit.w && mousey>=playButton.boxExit.y && mousey<=playButton.boxExit.y+ playButton.boxExit.h)
        {
            selectedButton = 2;
            playSelected = false;
            scoresSelected = false;
            exitSelected = true;
        }

        playButton.stateHolder(renderer, font, "Play", playSelected, scoresSelected, exitSelected);

             // Clear screen
        SDL_SetRenderDrawColor(renderer,
            mainMenuColor::background.r,
            mainMenuColor::background.g,
            mainMenuColor::background.b,
            mainMenuColor::background.a);

        SDL_RenderClear(renderer);
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);

        if (titleTexture)
        {
            int titleW, titleH;
            SDL_QueryTexture(titleTexture, NULL, NULL, &titleW, &titleH);
            SDL_Rect titleRect = {
                (1980 - titleW) / 2,
                110,
                titleW,
                titleH
            };
            SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        }

        const int lineY = start.y;
        const int cubeSize = 45;
        const int cubeX = 260;
        const int cubeGroundY = lineY - cubeSize;
        const float gravity = 1800.0f;
        const float jumpSpeed = -720.0f;
        const float trapSpeed = 430.0f;
        const int trapSpacing = 360;
        const int trapSize = 50;

        trapOffset += trapSpeed * deltaTime;
        if (trapOffset >= trapSpacing)
            trapOffset -= trapSpacing;

        bool trapNearCube = false;
        for (int i = 0; i < 7; i++)
        {
            int trapX = end.x - (int)trapOffset - (i * trapSpacing);
            if (trapX > cubeX + cubeSize && trapX < cubeX + cubeSize + 170)
                trapNearCube = true;
        }

        if (trapNearCube && cubeY >= cubeGroundY)
            cubeVelocity = jumpSpeed;

        cubeY += cubeVelocity * deltaTime;
        cubeVelocity += gravity * deltaTime;
        if (cubeY > cubeGroundY)
        {
            cubeY = cubeGroundY;
            cubeVelocity = 0.0f;
        }

        SDL_Rect cube = {cubeX, (int)cubeY, cubeSize, cubeSize};

        SDL_SetRenderDrawColor(renderer, 255, 127, 0, 255);
        SDL_RenderFillRect(renderer, &cube);
        SDL_SetRenderDrawColor(renderer, 40, 22, 18, 255);
        SDL_RenderDrawRect(renderer, &cube);

        for (int i = 0; i < 7; i++)
        {
            int trapX = end.x - (int)trapOffset - (i * trapSpacing);
            if (trapX < -trapSize || trapX > end.x + trapSize)
                continue;

            filledTrigonRGBA(renderer,
                trapX, lineY,
                trapX + trapSize, lineY,
                trapX + trapSize / 2, lineY - trapSize,
                92, 52, 40, 255);
            trigonRGBA(renderer,
                trapX, lineY,
                trapX + trapSize, lineY,
                trapX + trapSize / 2, lineY - trapSize,
                255, 127, 0, 255);
        }

        // Render button
        playButton.render(renderer);

        SDL_RenderPresent(renderer);
    }

    if (titleTexture)
        SDL_DestroyTexture(titleTexture);

    return false;
}
