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
    std::cout<<"Registration<<endl;
    SDL_Window* window=nullptr;
    SDL_Renderer* renderer=nullptr;
    std::cin.ignore();
   

    if(!init(window,renderer))
    {
        std::cerr<<"FAILD TO START "<<std::endl;
        return -1;
    }

     ground_Class ground(100,900,500,100,renderer,"Images_textures/ground.jpeg","Images_textures/underGround.jpeg");

    bool quit=false;   
    SDL_Event eventManager;
    while(!quit)
    {

        while(SDL_PollEvent(&eventManager)!=0)
        {
            if(eventManager.type==SDL_QUIT)
            {
                quit=true;
            }
        }

        SDL_SetRenderDrawColor(renderer,135,206,235,255);  
        SDL_RenderClear(renderer);

        ground.render(renderer);

        SDL_RenderPresent(renderer);

    }
    close(window,renderer);
    return 0;

}



