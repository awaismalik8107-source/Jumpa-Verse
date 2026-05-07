#include "object.h"
#include "functions.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    struct PlayerScoreRow
    {
        std::string username;
        int highestScore = 0;
    };

    bool pointInside(const SDL_Rect& rect, int x, int y)
    {
        return x >= rect.x && x <= rect.x + rect.w &&
               y >= rect.y && y <= rect.y + rect.h;
    }

    TTF_Font* openFont(int size, TTF_Font* fallback)
    {
        TTF_Font* font = TTF_OpenFont("debrosee-font/Debrosee-ALPnL.ttf", size);
        return font ? font : fallback;
    }

    void drawText(SDL_Renderer* renderer,
                  TTF_Font* font,
                  const std::string& text,
                  SDL_Color color,
                  SDL_Rect bounds,
                  bool center = true)
    {
        if (!renderer || !font || text.empty())
        {
            return;
        }

        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
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
        float scaleW = textW > 0 ? (float)bounds.w / (float)textW : 1.0f;
        float scaleH = textH > 0 ? (float)bounds.h / (float)textH : 1.0f;
        float scale = scaleW < scaleH ? scaleW : scaleH;
        if (scale > 1.0f)
        {
            scale = 1.0f;
        }

        int drawW = (int)(textW * scale);
        int drawH = (int)(textH * scale);
        SDL_Rect target = {
            center ? bounds.x + (bounds.w - drawW) / 2 : bounds.x,
            bounds.y + (bounds.h - drawH) / 2,
            drawW,
            drawH
        };

        SDL_RenderCopy(renderer, texture, nullptr, &target);
        SDL_DestroyTexture(texture);
    }

    std::vector<PlayerScoreRow> readPlayerScores()
    {
        std::vector<PlayerScoreRow> rows;
        std::ifstream file("player.csv");
        std::string line;

        while (std::getline(file, line))
        {
            std::stringstream lineStream(line);
            std::string username;
            std::string password;
            std::string scoreText;

            std::getline(lineStream, username, ',');
            std::getline(lineStream, password, ',');
            if (username.empty())
            {
                continue;
            }

            int highestScore = 0;
            while (std::getline(lineStream, scoreText, ','))
            {
                std::stringstream scoreStream(scoreText);
                int score = 0;
                if (scoreStream >> score && score > highestScore)
                {
                    highestScore = score;
                }
            }

            rows.push_back(PlayerScoreRow{username, highestScore});
        }

        std::sort(rows.begin(), rows.end(),
                  [](const PlayerScoreRow& left, const PlayerScoreRow& right)
                  {
                      return left.highestScore > right.highestScore;
                  });

        return rows;
    }
}

ScoreMenu::ScoreMenu() = default;

void ScoreMenu::run(SDL_Renderer* renderer, TTF_Font* font)
{
    TTF_Font* titleFont = openFont(86, font);
    TTF_Font* headerFont = openFont(42, font);
    TTF_Font* rowFont = openFont(34, font);
    TTF_Font* digitFont = TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Bold.ttf", 34);
    if (!digitFont)
    {
        digitFont = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", 34);
    }
    if (!digitFont)
    {
        digitFont = rowFont;
    }

    std::vector<PlayerScoreRow> rows = readPlayerScores();
    SDL_Event event;
    bool running = true;
    bool backHover = false;
    int scrollOffset = 0;

    while (running)
    {
        int screenW = GAME_WIDTH;
        int screenH = GAME_HEIGHT;
        getScreenSize(renderer, screenW, screenH);

        SDL_Rect backButton = {screenW / 2 - 150, screenH - 150, 300, 78};
        const int maxRows = 12;
        int maxScroll = rows.size() > (size_t)maxRows ? (int)rows.size() - maxRows : 0;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exitb = true;
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN && scrollOffset < maxScroll)
            {
                scrollOffset++;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP && scrollOffset > 0)
            {
                scrollOffset--;
            }
            if (event.type == SDL_MOUSEWHEEL)
            {
                scrollOffset -= event.wheel.y;
                if (scrollOffset < 0)
                    scrollOffset = 0;
                if (scrollOffset > maxScroll)
                    scrollOffset = maxScroll;
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                backHover = pointInside(backButton, event.motion.x, event.motion.y);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                pointInside(backButton, event.button.x, event.button.y))
            {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer,
                               mainMenuColor::background.r,
                               mainMenuColor::background.g,
                               mainMenuColor::background.b,
                               mainMenuColor::background.a);
        SDL_RenderClear(renderer);

        drawText(renderer, titleFont, "Scores", SDL_Color{255, 127, 0, 255},
                 SDL_Rect{screenW / 2 - 260, 95, 520, 105});

        int tableW = 980;
        int tableX = (screenW - tableW) / 2;
        int headerY = 265;
        SDL_Rect header = {tableX, headerY, tableW, 74};
        SDL_SetRenderDrawColor(renderer, 255, 127, 0, 255);
        SDL_RenderDrawRect(renderer, &header);
        drawText(renderer, headerFont, "USERNAME", SDL_Color{255, 255, 255, 255},
                 SDL_Rect{tableX + 36, headerY + 12, 420, 50}, false);
        drawText(renderer, headerFont, "HIGHEST SCORE", SDL_Color{255, 255, 255, 255},
                 SDL_Rect{tableX + tableW - 430, headerY + 12, 395, 50}, false);

        if (rows.empty())
        {
            drawText(renderer, rowFont, "No registered players found", SDL_Color{192, 192, 192, 255},
                     SDL_Rect{screenW / 2 - 360, 470, 720, 52});
        }
        else
        {
            const int rowH = 58;
            int visibleRows = (int)rows.size() - scrollOffset;
            int rowCount = visibleRows < maxRows ? visibleRows : maxRows;
            for (int i = 0; i < rowCount; i++)
            {
                int rowIndex = scrollOffset + i;
                int y = headerY + 96 + i * rowH;
                SDL_SetRenderDrawColor(renderer, i % 2 == 0 ? 42 : 48, i % 2 == 0 ? 42 : 48, i % 2 == 0 ? 42 : 48, 255);
                SDL_Rect rowRect = {tableX, y, tableW, rowH - 6};
                SDL_RenderFillRect(renderer, &rowRect);

                drawText(renderer, rowFont, rows[rowIndex].username, SDL_Color{255, 255, 255, 255},
                         SDL_Rect{tableX + 36, y + 7, 440, 42}, false);
                drawText(renderer, digitFont, std::to_string(rows[rowIndex].highestScore), SDL_Color{255, 127, 0, 255},
                         SDL_Rect{tableX + tableW - 250, y + 7, 215, 42}, false);
            }
        }

        SDL_Color fill = backHover ? SDL_Color{255, 127, 0, 255}
                                   : SDL_Color{192, 192, 192, 255};
        SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
        SDL_RenderFillRect(renderer, &backButton);
        drawText(renderer, headerFont, "Back", SDL_Color{30, 30, 30, 255}, backButton);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (titleFont != font) TTF_CloseFont(titleFont);
    if (headerFont != font) TTF_CloseFont(headerFont);
    if (rowFont != font) TTF_CloseFont(rowFont);
    if (digitFont != rowFont) TTF_CloseFont(digitFont);
}

void runScoreMenu(SDL_Renderer* renderer, TTF_Font* font)
{
    ScoreMenu screen;
    screen.run(renderer, font);
}
