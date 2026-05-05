#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include<iostream>
#include<SDL2/SDL.h>
#include <iterator>
#include<vector>
#include<string.h>
#include<vector>
#include<algorithm>//For basic algorithim like sorting 
#include"object.h"
#include<SDL2/SDL_image.h>
#include "functions.h"
#include<stdlib.h>
#include<SDL2/SDL_ttf.h>
float cammeraDash=0;
bool exitb = false;
int main()
{
    SDL_Window* window=nullptr;
    SDL_Renderer* renderer=nullptr;
     

    player player1;
   

    if(!init(window,renderer))
    {
        std::cerr<<"FAILD TO START "<<std::endl;
        return -1;
    }

    float cammera_offSet=0.5f; //no 2d camera in sdl2 so we create an illusion 
    //  ground_Class ground(100,900,500,100,renderer,"Images_textures/ground.jpeg","Images_textures/underGround.jpeg");

    //    ground=groundRandomgenerator(ground,renderer);
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "Images_textures/ground.jpeg");
    SDL_Texture* ugTex = IMG_LoadTexture(renderer, "Images_textures/underGround.jpeg");
    TTF_Font *font =TTF_OpenFont("debrosee-font/Debrosee-ALPnL.ttf",100);
    std::vector<trapSpike> trap;
    std::vector<ground_Class> ground;
    Score score;
    GameOver gameOver;
    auto resetGameplay = [&]()
    {
        player1 = player();
        cammera_offSet = 0.5f;
        trap.clear();
        ground = ground_generator(renderer, groundTex, ugTex);
        groundRandomgenerator(renderer, groundTex, ugTex, ground, trap);
        // ground.push_back(temp);
        resetScore(score);
        gameOver.reset();
    };

    initializeScore(score);
    resetGameplay();
    bool quit=false;   
    SDL_Event eventManager;
     Uint32 lastTime = SDL_GetTicks();
    int i=0;
    int count=0;
    const float targetFrameTime = 1000.0f / 60.0f; // 16.67 ms
    bool menu=false;
    while(!quit)
    {

       Uint32 frameStart = SDL_GetTicks();

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        float speed = 2.0f; // pixels per second
     
    while(!menu && !quit)
    {
    menu=menuScreen(renderer,font);
    if(exitb==true)
    {
        quit=true;
        continue;
    }
    resetGameplay();
    lastTime = SDL_GetTicks();
    }

    if (deltaTime > 0.05f)
    deltaTime = 0.05f;

        

        // if (cammera_offSet < -10.0f)
        //     cammera_offSet = -10.0f;

        if (cammera_offSet > -9.0f)
        {
            cammera_offSet -= speed * deltaTime;
        }

         i++;
      

        while(SDL_PollEvent(&eventManager)!=0)
        {
            if(eventManager.type==SDL_QUIT)
            {
                quit=true;
            }
        }
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
        player1.fallOffest=cammera_offSet;
        physicsDevelop(player1, ground, deltaTime, keyboardState);
        if (playerFellOff(player1))
        {
            gameOver.trigger(DeathCause::FellOff, score.value());
        }
        else if (playerHitTrap(player1, trap))
        {
            gameOver.trigger(DeathCause::HitTrap, score.value());
        }

        if (gameOver.isActive())
        {
            saveFinalScore(gameOver);
            GameOverAction action = runGameOverScreen(renderer, font, gameOver);
            if (exitb)
            {
                quit = true;
                continue;
            }

            resetGameplay();
            lastTime = SDL_GetTicks();

            if (action == GameOverAction::TryAgain)
            {
                menu = true;
            }
            else if (action == GameOverAction::ReturnToMenu)
            {
                menu = false;
            }

            continue;
        }

        bool playerAlive = !gameOver.isActive();
        updateScore(score, deltaTime, playerAlive);
        

        SDL_SetRenderDrawColor(renderer,135,206,235,255);  
        SDL_RenderClear(renderer);
       
        if(ground.size()<30)
        {
        groundRandomgenerator(renderer,groundTex,ugTex,ground,trap);
        }
        freeMemory(ground);  
        
        trapFreeMemory(trap);     
 
        cameraMovmentObj(ground,cammera_offSet+cammeraDash,trap);
        
        for(int j=0;j<ground.size();j++)
        {
            ground[j].render(renderer);
        }

        for(int j=0;j<trap.size();j++)
        {
             trap[j].render(renderer);
        }
        
        player1.Render(renderer);
        renderScore(score, renderer, font);
          

        SDL_RenderPresent(renderer);

        static int frames = 0;
    static Uint32 fpsTimer = SDL_GetTicks();

    frames++;

    Uint32 frameTime = SDL_GetTicks() - frameStart;

    if (frameTime < targetFrameTime)
    {
        SDL_Delay((Uint32)(targetFrameTime - frameTime));
    }


    if (SDL_GetTicks() - fpsTimer >= 1000)
    {
        std::cout << "\rFPS: " << frames 
          << "  Offset: " << cammera_offSet<<" "<<cammeraDash
          << std::flush;
        frames = 0;
        
        fpsTimer = SDL_GetTicks();
    }

    
    }
    close(window,renderer);
    return 0;

}
