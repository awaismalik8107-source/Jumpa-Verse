#include<iostream>
#include<vector>
#include<SDL2/SDL.h>
#include "object.h"
#include "functions.h"

void cameraMovmentObj(std::vector<ground_Class>& ground, float cameraOffset)
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