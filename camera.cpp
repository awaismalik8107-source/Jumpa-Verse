#include<iostream>
#include<vector>
#include<SDL2/SDL.h>
#include "object.h"
#include "functions.h"

void cameraMovmentObj(std::vector<ground_Class>& ground, float cameraOffset, std::vector<trapSpike>& trap)
{
   
    int intOffset = (int)cameraOffset;
    if (intOffset == 0 && cameraOffset != 0)
    {
        // If the offset is fractional but less than 1, we might want to still move
        // but since x is int, we can't. However, we can at least ensure ground and traps move together.
    }

    for (size_t i = 0; i < ground.size(); i++)
    {
        ground[i].seterForElements(
            ground[i].x + intOffset,
            ground[i].y,
            ground[i].w,
            ground[i].h
        );
    }

    for (size_t i = 0; i < trap.size(); i++)
    {
        trap[i].setterNew(trap[i].x + intOffset);
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