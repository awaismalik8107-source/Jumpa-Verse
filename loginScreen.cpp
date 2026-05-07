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
    const std::string playerFile = "player.csv";

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

    void drawButton(SDL_Renderer* renderer,
                    TTF_Font* font,
                    const SDL_Rect& rect,
                    const std::string& text,
                    bool selected)
    {
        SDL_Color fill = selected ? SDL_Color{255, 127, 0, 255}
                                  : SDL_Color{192, 192, 192, 255};
        SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
        SDL_RenderFillRect(renderer, &rect);
        drawText(renderer, font, text, SDL_Color{30, 30, 30, 255}, rect);
    }

    void drawInput(SDL_Renderer* renderer,
                   TTF_Font* labelFont,
                   TTF_Font* inputFont,
                   const SDL_Rect& rect,
                   const std::string& label,
                   const std::string& value,
                   bool password,
                   bool active)
    {
        SDL_Rect labelRect = {rect.x, rect.y - 54, rect.w, 44};
        drawText(renderer, labelFont, label, SDL_Color{255, 127, 0, 255}, labelRect, false);

        SDL_SetRenderDrawColor(renderer, 46, 46, 46, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, active ? 255 : 192,
                               active ? 127 : 192,
                               active ? 0 : 192,
                               255);
        SDL_RenderDrawRect(renderer, &rect);

        std::string visible = password ? std::string(value.size(), '*') : value;
        SDL_Rect textRect = {rect.x + 18, rect.y + 8, rect.w - 36, rect.h - 16};
        drawText(renderer, inputFont, visible, SDL_Color{255, 255, 255, 255}, textRect, false);
    }

    bool validateLogin(const std::string& username,
                       const std::string& password,
                       std::string& message)
    {
        if (username.empty() || password.empty())
        {
            message = "Username and password cannot be empty";
            return false;
        }

        std::ifstream file(playerFile);
        if (!file)
        {
            message = "No registered players found";
            return false;
        }

        const std::string inputUsername = toUpperUsername(username);
        std::string line;
        while (std::getline(file, line))
        {
            std::stringstream lineStream(line);
            std::string savedUsername;
            std::string savedPassword;

            std::getline(lineStream, savedUsername, ',');
            std::getline(lineStream, savedPassword, ',');

            if (toUpperUsername(savedUsername) == inputUsername)
            {
                if (savedPassword == password)
                {
                    message.clear();
                    return true;
                }

                message = "Incorrect password";
                return false;
            }
        }

        message = "Username not found";
        return false;
    }
}

std::string currentPlayerUsername;

LoginScreen::LoginScreen() = default;

LoginScreenAction LoginScreen::run(SDL_Renderer* renderer, TTF_Font* font)
{
    TTF_Font* titleFont = openFont(90, font);
    TTF_Font* formFont = openFont(42, font);
    TTF_Font* inputFont = openFont(38, font);
    TTF_Font* passwordFont = TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Bold.ttf", 38);
    if (!passwordFont)
    {
        passwordFont = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", 38);
    }
    if (!passwordFont)
    {
        passwordFont = inputFont;
    }
    TTF_Font* smallFont = openFont(30, font);

    SDL_StartTextInput();

    bool running = true;
    SDL_Event event;
    int selectedButton = 0;

    while (running)
    {
        int screenW = GAME_WIDTH;
        int screenH = GAME_HEIGHT;
        getScreenSize(renderer, screenW, screenH);

        const int formW = 680;
        const int inputH = 72;
        const int buttonW = 300;
        const int buttonH = 82;
        int formX = (screenW - formW) / 2;
        SDL_Rect usernameBox = {formX, 360, formW, inputH};
        SDL_Rect passwordBox = {formX, 510, formW, inputH};
        SDL_Rect loginButton = {screenW / 2 - buttonW - 24, 665, buttonW, buttonH};
        SDL_Rect backButton = {screenW / 2 + 24, 665, buttonW, buttonH};
        SDL_Rect registerText = {screenW / 2 - 440, 840, 880, 42};

        while (SDL_PollEvent(&event))
        {
            if (handleWindowControlEvent(renderer, event))
            {
                continue;
            }

            if (event.type == SDL_QUIT)
            {
                SDL_StopTextInput();
                if (titleFont != font) TTF_CloseFont(titleFont);
                if (formFont != font) TTF_CloseFont(formFont);
                if (inputFont != font) TTF_CloseFont(inputFont);
                if (passwordFont != inputFont) TTF_CloseFont(passwordFont);
                if (smallFont != font) TTF_CloseFont(smallFont);
                return LoginScreenAction::Quit;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mx = event.button.x;
                int my = event.button.y;
                if (pointInside(usernameBox, mx, my))
                {
                    activeField = 0;
                }
                else if (pointInside(passwordBox, mx, my))
                {
                    activeField = 1;
                }
                else if (pointInside(loginButton, mx, my))
                {
                    if (validateLogin(username, password, message))
                    {
                        currentPlayerUsername = username;
                        SDL_StopTextInput();
                        if (titleFont != font) TTF_CloseFont(titleFont);
                        if (formFont != font) TTF_CloseFont(formFont);
                        if (inputFont != font) TTF_CloseFont(inputFont);
                        if (passwordFont != inputFont) TTF_CloseFont(passwordFont);
                        if (smallFont != font) TTF_CloseFont(smallFont);
                        return LoginScreenAction::LoginSuccess;
                    }
                }
                else if (pointInside(backButton, mx, my))
                {
                    SDL_StopTextInput();
                    if (titleFont != font) TTF_CloseFont(titleFont);
                    if (formFont != font) TTF_CloseFont(formFont);
                    if (inputFont != font) TTF_CloseFont(inputFont);
                    if (passwordFont != inputFont) TTF_CloseFont(passwordFont);
                    if (smallFont != font) TTF_CloseFont(smallFont);
                    return LoginScreenAction::Back;
                }
                else if (pointInside(registerText, mx, my))
                {
                    RegisterScreenAction action = runRegisterScreen(renderer, font);
                    if (action == RegisterScreenAction::Quit)
                    {
                        SDL_StopTextInput();
                        if (titleFont != font) TTF_CloseFont(titleFont);
                        if (formFont != font) TTF_CloseFont(formFont);
                        if (inputFont != font) TTF_CloseFont(inputFont);
                        if (passwordFont != inputFont) TTF_CloseFont(passwordFont);
                        if (smallFont != font) TTF_CloseFont(smallFont);
                        return LoginScreenAction::Quit;
                    }
                    SDL_StartTextInput();
                    message = action == RegisterScreenAction::Registered ? "Registration complete. Please login" : message;
                }
            }

            if (event.type == SDL_MOUSEMOTION)
            {
                int mx = event.motion.x;
                int my = event.motion.y;
                if (pointInside(loginButton, mx, my))
                    selectedButton = 0;
                else if (pointInside(backButton, mx, my))
                    selectedButton = 1;
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    SDL_StopTextInput();
                    if (titleFont != font) TTF_CloseFont(titleFont);
                    if (formFont != font) TTF_CloseFont(formFont);
                    if (inputFont != font) TTF_CloseFont(inputFont);
                    if (passwordFont != inputFont) TTF_CloseFont(passwordFont);
                    if (smallFont != font) TTF_CloseFont(smallFont);
                    return LoginScreenAction::Back;
                }
                if (event.key.keysym.sym == SDLK_TAB)
                {
                    activeField = (activeField + 1) % 2;
                }
                else if (event.key.keysym.sym == SDLK_BACKSPACE)
                {
                    std::string& target = activeField == 0 ? username : password;
                    if (!target.empty())
                    {
                        target.pop_back();
                    }
                }
                else if (event.key.keysym.sym == SDLK_RETURN)
                {
                    if (validateLogin(username, password, message))
                    {
                        currentPlayerUsername = username;
                        SDL_StopTextInput();
                        if (titleFont != font) TTF_CloseFont(titleFont);
                        if (formFont != font) TTF_CloseFont(formFont);
                        if (inputFont != font) TTF_CloseFont(inputFont);
                        if (passwordFont != inputFont) TTF_CloseFont(passwordFont);
                        if (smallFont != font) TTF_CloseFont(smallFont);
                        return LoginScreenAction::LoginSuccess;
                    }
                }
            }

            if (event.type == SDL_TEXTINPUT)
            {
                std::string& target = activeField == 0 ? username : password;
                if (target.size() < 32)
                {
                    target += event.text.text;
                }
            }
        }

        int mouseX = 0;
        int mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);
        bool registerHover = pointInside(registerText, mouseX, mouseY);

        SDL_SetRenderDrawColor(renderer,
                               mainMenuColor::background.r,
                               mainMenuColor::background.g,
                               mainMenuColor::background.b,
                               mainMenuColor::background.a);
        SDL_RenderClear(renderer);

        drawText(renderer, titleFont, "Login", SDL_Color{255, 127, 0, 255},
                 SDL_Rect{screenW / 2 - 260, 115, 520, 110});

        drawInput(renderer, formFont, inputFont, usernameBox, "Username", username, false, activeField == 0);
        drawInput(renderer, formFont, passwordFont, passwordBox, "Password", password, false, activeField == 1);
        drawButton(renderer, formFont, loginButton, "Login", selectedButton == 0);
        drawButton(renderer, formFont, backButton, "Back", selectedButton == 1);

        drawText(renderer, smallFont, message, SDL_Color{255, 127, 0, 255},
                 SDL_Rect{screenW / 2 - 430, 770, 860, 44});
        drawText(renderer, smallFont, "If you don't have an account click here to register",
                 registerHover ? SDL_Color{255, 127, 0, 255} : SDL_Color{255, 255, 255, 255},
                 registerText);
        renderWindowControls(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
    if (titleFont != font) TTF_CloseFont(titleFont);
    if (formFont != font) TTF_CloseFont(formFont);
    if (inputFont != font) TTF_CloseFont(inputFont);
    if (passwordFont != inputFont) TTF_CloseFont(passwordFont);
    if (smallFont != font) TTF_CloseFont(smallFont);
    return LoginScreenAction::Back;
}

LoginScreenAction runLoginScreen(SDL_Renderer* renderer, TTF_Font* font)
{
    LoginScreen screen;
    return screen.run(renderer, font);
}
