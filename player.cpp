#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include<iostream>
#include<SDL2/SDL.h>
#include <iterator>
#include <ostream>
#include<vector>
#include<string.h>
#include<vector>
#include<algorithm>//For basic algorithim like sorting 
#include"object.h"
#include<SDL2/SDL_image.h>
#include "functions.h"
#include<stdlib.h>
#include<SDL2/SDL_ttf.h>
#include<limits>

void physicsDevelop(player& player1,
                    std::vector<ground_Class>& g1,
                    float deltaTime,
                    const Uint8* keyboardState)
{
    if (deltaTime <= 0.0f)
    {
        return;
    }

    bool jumpPressed = keyboardState[SDL_SCANCODE_SPACE] ||
                       keyboardState[SDL_SCANCODE_W] ||
                       keyboardState[SDL_SCANCODE_UP];

    if (jumpPressed && player1.onGround)
    {
        player1.jumpVelocity = player1.jumpPower;
        player1.onGround = false;
    }

    SDL_Rect previous = player1.character;
    int nextGroundIndex = -1;
    int nextGroundDistance = std::numeric_limits<int>::max();

    player1.jumpVelocity += player1.gravity * deltaTime;
    player1.character.y += static_cast<int>(player1.jumpVelocity * deltaTime);
    player1.onGround = false;

    for (size_t i = 0; i < g1.size(); i++)
    {
        SDL_Rect groundRect = {g1[i].x, g1[i].y, g1[i].w, g1[i].h};
        int playerRight = player1.character.x + player1.character.w;
        int distanceToNextGround = groundRect.x - playerRight;

        // The next ground must start after the player's right side.
        // If we compare with player.x, the current ground can be selected
        // when the player is standing near its left side.
        if (distanceToNextGround > 0 && distanceToNextGround < nextGroundDistance)
        {
            nextGroundDistance = distanceToNextGround;
            if(g1.size()>i+1)
            nextGroundIndex = static_cast<int>(i+1);
        }

        bool overlapX = (player1.character.x + player1.character.w > groundRect.x) &&
                        (player1.character.x < groundRect.x + groundRect.w);
        bool wasAboveGround = previous.y + previous.h <= groundRect.y;
        bool crossedGround = player1.character.y + player1.character.h >= groundRect.y;

        if (overlapX && wasAboveGround && crossedGround && player1.jumpVelocity >= 0.0f)
        {
            player1.character.y = groundRect.y - player1.character.h;
            player1.jumpVelocity = 0.0f;
            player1.onGround = true;
            break;

        }
    }

    player1.syncHitBox();
}
