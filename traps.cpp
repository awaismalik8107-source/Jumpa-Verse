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

void trapTriangle(SDL_Renderer* renderer, std::vector<trapSpike>& trap, std::vector<ground_Class>& ground, int i, int maxTraps)
{
    if (ground[i].first_ground || ground[i].trapCount >= maxTraps)
    {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    // Add a safety limit to prevent infinite looping if conditions are hard to meet
    int attempts = 0;
    const int maxAttempts = 100;

    while (attempts < maxAttempts)
    {
        attempts++;
        std::uniform_int_distribution<> chances(0, 1000);

        // Probability check
        if (chances(gen) < 100) // Adjusted probability for more consistent generation
        {
            std::uniform_int_distribution<> distp(ground[i].x, (ground[i].x + ground[i].w) - 100);
            int spikeX = distp(gen);

            // Check for overlap with existing traps
            bool overlapping = false;
            for (const auto& existingTrap : trap)
            {
                if (spikeX - existingTrap.x <= 200 && spikeX - existingTrap.x >= -200)
                {
                    overlapping = true;
                    break;
                }
            }

            if (!overlapping && ground[i].trapCount < maxTraps)
            {
                ground[i].trapCount++;
                trap.emplace_back(spikeX, ground[i].y, 50, 50);
                return; // Successfully added one trap
            }
        }
    }
}

void trapFreeMemory(std::vector<trapSpike>& trap)
{
    for (int i = 0; i < (int)trap.size(); )
    {
        // Remove traps that are fully off-screen to the left
        if (trap[i].x + 50 < -200) 
            trap.erase(trap.begin() + i);
        else
            i++;
    }
}
