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
#include "functions.h"

/*



*/



int main()
{
    SDL_Window* window=nullptr;
    SDL_Renderer* renderer=nullptr;

   

    if(!init(window,renderer))
    {
        std::cerr<<"FAILD TO START "<<std::endl;
        return -1;
    }

    

    //  ground_Class ground(100,900,500,100,renderer,"Images_textures/ground.jpeg","Images_textures/underGround.jpeg");

    //    ground=groundRandomgenerator(ground,renderer);
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "Images_textures/ground.jpeg");
    SDL_Texture* ugTex = IMG_LoadTexture(renderer, "Images_textures/underGround.jpeg");
    std::vector<ground_Class> ground = ground_generator(renderer,groundTex,ugTex);
    ground_Class temp=groundRandomgenerator(renderer,groundTex,ugTex,ground);
    ground.push_back(temp);
    bool quit=false;   
    SDL_Event eventManager;
    int i=0;
    while(!quit)
    {
        i++;
        while(SDL_PollEvent(&eventManager)!=0)
        {
            if(eventManager.type==SDL_QUIT)
            {
                quit=true;
            }
        }

        SDL_SetRenderDrawColor(renderer,135,206,235,255);  
        SDL_RenderClear(renderer);

        if(i%=60)
        {        
        temp=groundRandomgenerator(renderer,groundTex,ugTex,ground);
        ground.push_back(temp);
        }
        for(int i=0;i<ground.size();i++)
        {
        ground[i].render(renderer);
        }

        SDL_RenderPresent(renderer);

    }
    close(window,renderer);
    return 0;

}



