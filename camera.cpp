#include<iostream>
#include<vector>
#include<SDL2/SDL.h>
#include "object.h"
#include "functions.h"

void cameraMovmentObj(std::vector<ground_Class>& ground, float cameraOffset,std::vector<trapSpike>&trap)
{
    for (int i = 0; i < ground.size(); i++)
    {
        ground[i].seterForElements(
            ground[i].x + (int)cameraOffset,
            ground[i].y,
            ground[i].w,
            ground[i].h

        );


    }
}

void renderTrap(SDL_Renderer* renderer,
                std::vector<trapSpike>& trap,
                float cameraOffset)
{
    for (size_t i = 0; i < trap.size(); i++)
    {
        trap[i].setterNew(trap[i].x+(int)cameraOffset);
    }
}

// std::vector<ground_Class>(std::vector<ground_Class> ground, float cameraOffset)
// {
//     for (int i = 0; i < ground.size(); i++)
//     {
//         ground[i].seterForElements(
//             ground[i].x + (int)cameraOffset,
//             ground[i].y,
//             ground[i].w,
//             ground[i].h
//         );
//     }
//return ground;
// }