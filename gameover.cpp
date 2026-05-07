#include "object.h"
#include "functions.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    SDL_Rect trapVisibleHitbox(const trapSpike& trap)
    {
        // The spike is rendered as a fixed 50x50 triangle with its base at trap.y.
        return SDL_Rect{trap.x, trap.y - 50, 50, 50};
    }

    bool rectsOverlap(const SDL_Rect& a, const SDL_Rect& b)
    {
        return SDL_HasIntersection(&a, &b) == SDL_TRUE;
    }

    void renderText(SDL_Renderer* renderer,
                    TTF_Font* font,
                    const std::string& text,
                    SDL_Color color,
                    const SDL_Rect& bounds)
    {
        if (!renderer || !font || text.empty())
        {
            return;
        }

        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surface)
        {
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture)
        {
            return;
        }

        int textW = 0;
        int textH = 0;
        SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
        float scaleW = (float)bounds.w / (float)textW;
        float scaleH = (float)bounds.h / (float)textH;
        float scale = scaleW < scaleH ? scaleW : scaleH;
        if (scale > 1.0f)
        {
            scale = 1.0f;
        }

        int drawW = (int)(textW * scale);
        int drawH = (int)(textH * scale);
        SDL_Rect target = {
            bounds.x + (bounds.w - drawW) / 2,
            bounds.y + (bounds.h - drawH) / 2,
            drawW,
            drawH
        };

        SDL_RenderCopy(renderer, texture, nullptr, &target);
        SDL_DestroyTexture(texture);
    }

    bool pointInside(const SDL_Rect& rect, int x, int y)
    {
        return x >= rect.x && x <= rect.x + rect.w &&
               y >= rect.y && y <= rect.y + rect.h;
    }

    void renderGameOver(SDL_Renderer* renderer,
                        TTF_Font* font,
                        const GameOver& gameOver,
                        int selectedButton)
    {
        int screenW = GAME_WIDTH;
        int screenH = GAME_HEIGHT;
        getScreenSize(renderer, screenW, screenH);

        SDL_SetRenderDrawColor(renderer, 24, 24, 24, 255);
        SDL_RenderClear(renderer);

        int stripH = screenH / 3;
        if (stripH < 320)
        {
            stripH = 320;
        }
        if (stripH > 420)
        {
            stripH = 420;
        }

        SDL_Rect strip = {0, (screenH - stripH) / 2, screenW, stripH};
        SDL_SetRenderDrawColor(renderer, 255, 127, 0, 255);
        SDL_RenderFillRect(renderer, &strip);

        SDL_Color darkText = {28, 22, 16, 255};
        SDL_Color lightText = {255, 255, 255, 255};
        static TTF_Font* digitFont = nullptr;
        if (!digitFont)
        {
            int digitPointSize = TTF_FontHeight(font);
            if (digitPointSize <= 0)
            {
                digitPointSize = 100;
            }

            digitFont = TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Bold.ttf", digitPointSize);
            if (!digitFont)
            {
                digitFont = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", digitPointSize);
            }
        }

        renderText(renderer, font, "Game Over", darkText,
                   SDL_Rect{screenW / 2 - 420, strip.y + 24, 840, 110});

        renderText(renderer, font, gameOver.causeText(), darkText,
                   SDL_Rect{screenW / 2 - 360, strip.y + 132, 720, 58});

        std::string finalScoreLabel = "Final Score:";
        std::string finalScoreValue = std::to_string(gameOver.finalScore());
        if (digitFont)
        {
            SDL_Surface* labelSurface = TTF_RenderUTF8_Blended(font, finalScoreLabel.c_str(), darkText);
            SDL_Surface* valueSurface = TTF_RenderUTF8_Blended(digitFont, finalScoreValue.c_str(), darkText);
            if (labelSurface && valueSurface)
            {
                SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
                SDL_Texture* valueTexture = SDL_CreateTextureFromSurface(renderer, valueSurface);
                if (labelTexture && valueTexture)
                {
                    int labelW = 0, labelH = 0, valueW = 0, valueH = 0;
                    SDL_QueryTexture(labelTexture, nullptr, nullptr, &labelW, &labelH);
                    SDL_QueryTexture(valueTexture, nullptr, nullptr, &valueW, &valueH);

                    const int maxTextH = 58;
                    float labelScale = labelH > 0 ? (float)maxTextH / (float)labelH : 1.0f;
                    float valueScale = valueH > 0 ? (float)maxTextH / (float)valueH : 1.0f;
                    int drawLabelW = (int)(labelW * labelScale);
                    int drawLabelH = maxTextH;
                    int drawValueW = (int)(valueW * valueScale);
                    int drawValueH = maxTextH;

                    const int spacing = 12;
                    int totalW = drawLabelW + spacing + drawValueW;
                    int x = screenW / 2 - totalW / 2;
                    int y = strip.y + 190;

                    SDL_Rect labelRect = {x, y, drawLabelW, drawLabelH};
                    SDL_Rect valueRect = {x + drawLabelW + spacing, y, drawValueW, drawValueH};
                    SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
                    SDL_RenderCopy(renderer, valueTexture, nullptr, &valueRect);
                }
                if (labelTexture) SDL_DestroyTexture(labelTexture);
                if (valueTexture) SDL_DestroyTexture(valueTexture);
            }
            if (labelSurface) SDL_FreeSurface(labelSurface);
            if (valueSurface) SDL_FreeSurface(valueSurface);
        }

        const int buttonW = 360;
        const int buttonH = 78;
        const int gap = 42;
        int buttonsY = strip.y + strip.h - buttonH - 42;
        SDL_Rect tryAgain = {screenW / 2 - buttonW - gap / 2, buttonsY, buttonW, buttonH};
        SDL_Rect menu = {screenW / 2 + gap / 2, buttonsY, buttonW, buttonH};

        SDL_Color selectedColor = {38, 38, 38, 255};
        SDL_Color normalColor = {74, 74, 74, 255};

        SDL_SetRenderDrawColor(renderer,
                               selectedButton == 0 ? selectedColor.r : normalColor.r,
                               selectedButton == 0 ? selectedColor.g : normalColor.g,
                               selectedButton == 0 ? selectedColor.b : normalColor.b,
                               255);
        SDL_RenderFillRect(renderer, &tryAgain);

        SDL_SetRenderDrawColor(renderer,
                               selectedButton == 1 ? selectedColor.r : normalColor.r,
                               selectedButton == 1 ? selectedColor.g : normalColor.g,
                               selectedButton == 1 ? selectedColor.b : normalColor.b,
                               255);
        SDL_RenderFillRect(renderer, &menu);

        renderText(renderer, font, "Try Again", lightText, tryAgain);
        renderText(renderer, font, "Return to Menu", lightText, menu);
        renderWindowControls(renderer);

        SDL_RenderPresent(renderer);
    }
}

bool playerFellOff(const player& player1, int fallY)
{
    if (fallY < 0)
    {
        fallY = currentScreenHeight + 120;
    }

    return player1.character.y >= fallY;
}

bool playerHitTrap(const player& player1, const std::vector<trapSpike>& trap)
{
    for (const trapSpike& spike : trap)
    {
        SDL_Rect spikeRect = trapVisibleHitbox(spike);
        if (rectsOverlap(player1.character, spikeRect))
        {
            return true;
        }
    }

    return false;
}

void saveFinalScore(const GameOver& gameOver, const std::string& path)
{
    std::ofstream file(path, std::ios::app);
    if (!file)
    {
        return;
    }

    file << gameOver.finalScore() << '\n';
}

void saveLoggedInPlayerScore(const GameOver& gameOver, const std::string& path)
{
    if (currentPlayerUsername.empty())
    {
        return;
    }

    std::ifstream input(path);
    if (!input)
    {
        return;
    }

    const std::string playerUsername = toUpperUsername(currentPlayerUsername);
    std::vector<std::string> lines;
    std::string line;
    bool playerFound = false;

    while (std::getline(input, line))
    {
        std::stringstream lineStream(line);
        std::string username;
        std::getline(lineStream, username, ',');

        if (toUpperUsername(username) == playerUsername)
        {
            line += "," + std::to_string(gameOver.finalScore());
            playerFound = true;
        }

        lines.push_back(line);
    }

    input.close();

    if (!playerFound)
    {
        return;
    }

    std::ofstream output(path, std::ios::trunc);
    if (!output)
    {
        return;
    }

    for (const std::string& savedLine : lines)
    {
        output << savedLine << '\n';
    }
}

GameOverAction runGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, GameOver& gameOver)
{
    int selectedButton = 0;
    SDL_Event event;

    while (gameOver.isActive())
    {
        while (SDL_PollEvent(&event))
        {
            if (handleWindowControlEvent(renderer, event))
            {
                continue;
            }

            if (event.type == SDL_QUIT)
            {
                exitb = true;
                return GameOverAction::ReturnToMenu;
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT)
                {
                    selectedButton = 1 - selectedButton;
                }
                else if (event.key.keysym.sym == SDLK_RETURN ||
                         event.key.keysym.sym == SDLK_SPACE)
                {
                    gameOver.reset();
                    return selectedButton == 0 ? GameOverAction::TryAgain
                                               : GameOverAction::ReturnToMenu;
                }
            }

            if (event.type == SDL_MOUSEMOTION)
            {
                int screenW = GAME_WIDTH;
        int screenH = GAME_HEIGHT;
        getScreenSize(renderer, screenW, screenH);
                int stripH = screenH / 3;
                if (stripH < 320)
                {
                    stripH = 320;
                }
                if (stripH > 420)
                {
                    stripH = 420;
                }

                const int buttonW = 360;
                const int buttonH = 78;
                const int gap = 42;
                int buttonsY = (screenH - stripH) / 2 + stripH - buttonH - 42;
                SDL_Rect tryAgain = {screenW / 2 - buttonW - gap / 2, buttonsY, buttonW, buttonH};
                SDL_Rect menu = {screenW / 2 + gap / 2, buttonsY, buttonW, buttonH};

                if (pointInside(tryAgain, event.motion.x, event.motion.y))
                {
                    selectedButton = 0;
                }
                else if (pointInside(menu, event.motion.x, event.motion.y))
                {
                    selectedButton = 1;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int screenW = GAME_WIDTH;
        int screenH = GAME_HEIGHT;
        getScreenSize(renderer, screenW, screenH);
                int stripH = screenH / 3;
                if (stripH < 320)
                {
                    stripH = 320;
                }
                if (stripH > 420)
                {
                    stripH = 420;
                }

                const int buttonW = 360;
                const int buttonH = 78;
                const int gap = 42;
                int buttonsY = (screenH - stripH) / 2 + stripH - buttonH - 42;
                SDL_Rect tryAgain = {screenW / 2 - buttonW - gap / 2, buttonsY, buttonW, buttonH};
                SDL_Rect menu = {screenW / 2 + gap / 2, buttonsY, buttonW, buttonH};

                if (pointInside(tryAgain, event.button.x, event.button.y))
                {
                    gameOver.reset();
                    return GameOverAction::TryAgain;
                }
                if (pointInside(menu, event.button.x, event.button.y))
                {
                    gameOver.reset();
                    return GameOverAction::ReturnToMenu;
                }
            }
        }

        renderGameOver(renderer, font, gameOver, selectedButton);
        SDL_Delay(16);
    }

    return GameOverAction::None;
}
