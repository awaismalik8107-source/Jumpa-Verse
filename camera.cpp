#include<iostream>
#include<vector>
#include<SDL2/SDL.h>
#include "object.h"
#include "functions.h"

std::vector<ground_Class> cameraMovmentObj(std::vector<ground_Class> ground,float cameraOffset)
{

    int camera=cameraOffset;
    for(int i=0;i<ground.size();i++)
    {
        ground[i].seterForElements(ground[i].x+camera,ground[i].y,ground[i].w,ground[i].h);
    }
    return ground;
}