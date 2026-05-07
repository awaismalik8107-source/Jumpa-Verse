#include "object.h"
#include "functions.h"

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string>

Score::Score()
{
    initialize();
}

Score::~Score() = default;

void Score::initialize()
{
    reset();
}

void Score::update(float deltaTime, bool playerAlive)
{
    if (!playerAlive || deltaTime <= 0.0f)
    {
        return;
    }

    timeAccumulator += deltaTime;
    if (timeAccumulator >= 1.0f)
    {
        score += 1;
        timeAccumulator = 0.0f;
    }
}

void Score::render(SDL_Renderer* renderer, TTF_Font* font)
{
    if (!renderer)
    {
        return;
    }

    static TTF_Font* labelFont = nullptr;
    if (!labelFont)
    {
        labelFont = TTF_OpenFont("debrosee-font/Debrosee-ALPnL.ttf", 26);
        if (!labelFont)
        {
            labelFont = font;
        }
    }

    static TTF_Font* digitFont = nullptr;
    if (!digitFont)
    {
        digitFont = TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Bold.ttf", 26);
        if (!digitFont)
        {
            digitFont = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", 26);
        }
    }

    if (!labelFont || !digitFont)
    {
        return;
    }

    int screenW = GAME_WIDTH;
        int screenH = GAME_HEIGHT;
        getScreenSize(renderer, screenW, screenH);

    const int margin = 28;
    const int paddingX = 18;
    const int paddingY = 10;
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color borderColor = {255, 127, 0, 255};
    std::string labelText = "SCORE:";
    std::string valueText = std::to_string(score);

    SDL_Surface* labelSurface = TTF_RenderUTF8_Blended(labelFont, labelText.c_str(), textColor);
    SDL_Surface* valueSurface = TTF_RenderUTF8_Blended(digitFont, valueText.c_str(), textColor);
    if (!labelSurface || !valueSurface)
    {
        if (labelSurface) SDL_FreeSurface(labelSurface);
        if (valueSurface) SDL_FreeSurface(valueSurface);
        return;
    }

    SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    SDL_Texture* valueTexture = SDL_CreateTextureFromSurface(renderer, valueSurface);
    SDL_FreeSurface(labelSurface);
    SDL_FreeSurface(valueSurface);
    if (!labelTexture || !valueTexture)
    {
        if (labelTexture) SDL_DestroyTexture(labelTexture);
        if (valueTexture) SDL_DestroyTexture(valueTexture);
        return;
    }

    int labelW = 0;
    int labelH = 0;
    int valueW = 0;
    int valueH = 0;
    SDL_QueryTexture(labelTexture, nullptr, nullptr, &labelW, &labelH);
    SDL_QueryTexture(valueTexture, nullptr, nullptr, &valueW, &valueH);

    const int spacing = 8;
    int boxW = labelW + spacing + valueW + paddingX * 2;
    int boxH = (labelH > valueH ? labelH : valueH) + paddingY * 2;
    int x = screenW - margin - boxW;
    if (x < margin)
    {
        x = margin;
    }

    SDL_Rect box = {x, margin, boxW, boxH};
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &box);

    SDL_Rect labelRect = {box.x + paddingX, box.y + paddingY, labelW, labelH};
    SDL_Rect valueRect = {box.x + paddingX + labelW + spacing, box.y + paddingY, valueW, valueH};
    SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
    SDL_RenderCopy(renderer, valueTexture, nullptr, &valueRect);
    SDL_DestroyTexture(labelTexture);
    SDL_DestroyTexture(valueTexture);
}

void Score::reset()
{
    score = 0;
    timeAccumulator = 0.0f;
}

int Score::value() const
{
    return score;
}

void initializeScore(Score& score)
{
    score.initialize();
}

void updateScore(Score& score, float deltaTime, bool playerAlive)
{
    score.update(deltaTime, playerAlive);
}

void renderScore(Score& score, SDL_Renderer* renderer, TTF_Font* font)
{
    score.render(renderer, font);
}

void resetScore(Score& score)
{
    score.reset();
}
