#include<iostream>
#include<vector>
#include<SDL2/SDL.h>
#include "object.h"
#include "functions.h"

std::vector<ground_Class> cameraMovmentObj(std::vector<ground_Class> ground,float cameraOffset)
{
    for(int i=0;i<ground.size();i++)
    {
        ground[i].x+=(int)cameraOffset;
        ground[i].rebuild();
    }
    return ground;
}