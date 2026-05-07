#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL2_gfxPrimitives.h>//to import trigons
#include<SDL2/SDL_ttf.h>

class hitBox
{
public:
    int x, y, w, h;

    hitBox(int x, int y, int w, int h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    hitBox();
    // inline optionBoxes::optionBoxes(int x, int y, int w, int h);
};

class ground_Class : public hitBox
{
public:
bool first_ground=false;
/*Characterrizing ground into 3 types small medium and big*/
    bool small=false;
    bool big=false;
    bool medium=false;
    int divw = 0;
    int trapCount=0;

    bool trapFill=false;
    std::vector<SDL_Rect> ground;
    std::vector<std::vector<SDL_Rect>> groundUnder;

    SDL_Texture* groundTexture = nullptr;  // shared (NOT owned)
    SDL_Texture* ugTexture = nullptr;      // shared (NOT owned)

    int perboxwidth = 50;
    int perboxheight = 50;

public:
    // Default constructor
    ground_Class() : hitBox(0, 0, 0, 0) {}

    // Main constructor (USES SHARED TEXTURES)
    ground_Class(int ax, int ay, int aw, int ah,
                 SDL_Texture* groundTex,
                 SDL_Texture* ugTex)
        
        : hitBox(ax, ay, aw, ah),
          groundTexture(groundTex),
          ugTexture(ugTex)
    {
        rebuild();
    }

    // Rebuild geometry only (NO SDL loading here)
    void rebuild()
    {
        divw = w / perboxwidth;
        int rows = h / perboxheight;

        ground.clear();
        groundUnder.clear();

        ground.resize(divw);
        groundUnder.resize(divw);

        for (int i = 0; i < divw; i++)
        {
            groundUnder[i].resize(rows);
        }

        for (int i = 0; i < divw; i++)
        {
            ground[i] = { x + i * perboxwidth, y, perboxwidth, perboxheight };
            for (int j = 0; j < rows; j++)
            {
                groundUnder[i][j] =
                {
                    x + i * perboxwidth,
                    y + (j + 1) * perboxheight,
                    perboxwidth,
                    perboxheight
                };
            }
        }
    }

    void seterForElements(int newX, int newY, int newW, int newH)
    {
        x = newX;
        y = newY;
        w = newW;
        h = newH;

        rebuild();
    }

    void render(SDL_Renderer* renderer)
    {
        if (!groundTexture || !ugTexture)
        {
            std::cerr << "Cannot render: texture is null!\n";
            return;
        }

        for (int i = 0; i < divw; i++)
        {
            SDL_RenderCopy(renderer, groundTexture, nullptr, &ground[i]);

            for (int j = 0; j < (int)groundUnder[i].size(); j++)
            {
                SDL_RenderCopy(renderer, ugTexture, nullptr, &groundUnder[i][j]);
            }
        }
    }

    
   
};


class trapSpike : public hitBox
{
public:
    trapSpike() : hitBox(0, 0, 0, 0) {}

    trapSpike(int x, int y, int w, int h) : hitBox(x, y, w, h) {}

    void setterNew(int x)
    {
        this->x = x;
    }

    void render(SDL_Renderer* renderer, float cameraOffset = 0)
    {
        int x1 = x + (int)cameraOffset;
        int y1 = y;
        
        // Visibility and Overflow Check:
        // SDL2_gfx uses int16_t (-32768 to 32767). 
        // We only render if it's reasonably near the screen to avoid overflow "streaks".
        if (x1 < -500 || x1 > 3000) 
        {
            return;
        }

        int x2 = x1 + 50; 
        int y2 = y1;
        int x3 = x1 + 25; 
        int y3 = y1 - 50; 

        filledTrigonRGBA(renderer,
                        x1, y1,
                        x2, y2,
                        x3, y3,
                        92, 52, 40, 255);

        trigonRGBA(renderer,
                  x1, y1,
                  x2, y2,
                  x3, y3,
                  40, 22, 18, 255);
    }

    void updatePosition(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
};

class Score
{
public:
    Score();
    ~Score();

    Score(const Score&) = delete;
    Score& operator=(const Score&) = delete;

    void initialize();
    void update(float deltaTime, bool playerAlive);
    void render(SDL_Renderer* renderer, TTF_Font* font);
    void reset();
    int value() const;

private:
    int score = 0;
    float timeAccumulator = 0.0f;
};

enum class DeathCause
{
    None,
    FellOff,
    HitTrap
};

enum class GameOverAction
{
    None,
    TryAgain,
    ReturnToMenu
};

enum class LoginScreenAction
{
    Back,
    LoginSuccess,
    Quit
};

enum class RegisterScreenAction
{
    Back,
    Registered,
    Quit
};

class GameOver
{
public:
    GameOver();

    void reset();
    void trigger(DeathCause cause, int score);
    bool isActive() const;
    DeathCause cause() const;
    int finalScore() const;
    std::string causeText() const;

private:
    bool active = false;
    DeathCause deathCause = DeathCause::None;
    int savedFinalScore = 0;
};

class mainMenuColor {
public:
    // Background color
    static SDL_Color background;

    // Button colors
    static SDL_Color buttonNormal;
    static SDL_Color buttonHover;
    static SDL_Color buttonActive;

    // Text color
    static SDL_Color text;
};

class LoginScreen
{
public:
    LoginScreen();
    LoginScreenAction run(SDL_Renderer* renderer, TTF_Font* font);

private:
    std::string username;
    std::string password;
    std::string message;
    int activeField = 0;
};

class RegisterScreen
{
public:
    RegisterScreen();
    RegisterScreenAction run(SDL_Renderer* renderer, TTF_Font* font);

private:
    std::string username;
    std::string password;
    std::string confirmPassword;
    std::string message;
    int activeField = 0;
};

class ScoreMenu
{
public:
    ScoreMenu();
    void run(SDL_Renderer* renderer, TTF_Font* font);
};


class optionBoxes : public hitBox
{
public:
    SDL_Texture* textTexture = nullptr;
    SDL_Texture* scoresTexture = nullptr;
    SDL_Texture* exitTexture = nullptr;

    bool state = false;
    bool stateScores = false;
    bool stateExit=false;

    SDL_Rect box;
    SDL_Rect boxScores;
    SDL_Rect boxExit;

    std::string scoresText = "Scores";
    std::string exitText = "Exit";
    int boxOffsetY = 30;

    optionBoxes(int x, int y, int w, int h)
        : hitBox(x, y, w, h)
    {
        box = {x, y, w, h};
        boxScores = {x, y + h + boxOffsetY, w, h};
        boxExit = {x, y + (h + boxOffsetY) * 2, w, h};
    }

    ~optionBoxes()
    {
        if (textTexture) SDL_DestroyTexture(textTexture);
        if (scoresTexture) SDL_DestroyTexture(scoresTexture);
        if (exitTexture) SDL_DestroyTexture(exitTexture);
    }

    void box_Initializer(SDL_Renderer* renderer, TTF_Font* font, std::string text)
    {
        if (!font)
        {
            std::cout << "Font is NULL!\n";
            return;
        }

        // cleanup old textures
        if (textTexture) SDL_DestroyTexture(textTexture);
        if (scoresTexture) SDL_DestroyTexture(scoresTexture);
        if (exitTexture) SDL_DestroyTexture(exitTexture);

        SDL_Color color = !state ? SDL_Color{255,127,0,255}
                                 : SDL_Color{192,192,192,255};

        SDL_Color colorScores = !stateScores ? SDL_Color{255,127,0,255}
                                 : SDL_Color{192,192,192,255};

        SDL_Color colorExit = !stateExit ? SDL_Color{255,127,0,255}
                                 : SDL_Color{192,192,192,255};

        // ---- MAIN TEXT ----
        SDL_Surface* surface1 = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surface1)
        {
            std::cout << "Surface1 error: " << TTF_GetError() << "\n";
            return;
        }


        textTexture = SDL_CreateTextureFromSurface(renderer, surface1);
        SDL_FreeSurface(surface1);

        // ---- SCORES TEXT ----
        SDL_Surface* surfaceScores = TTF_RenderText_Blended(font, scoresText.c_str(), colorScores);
        if (!surfaceScores)
        {
            std::cout << "Scores surface error: " << TTF_GetError() << "\n";
            return;
        }

        scoresTexture = SDL_CreateTextureFromSurface(renderer, surfaceScores);
        SDL_FreeSurface(surfaceScores);

        // ---- EXIT TEXT ----
        SDL_Surface* surface2 = TTF_RenderText_Blended(font, exitText.c_str(), colorExit);
        if (!surface2)
        {
            std::cout << "Surface2 error: " << TTF_GetError() << "\n";
            return;
        }

        exitTexture = SDL_CreateTextureFromSurface(renderer, surface2);
        SDL_FreeSurface(surface2);
    }

    void render(SDL_Renderer* renderer)
    {
        SDL_Color color = state ? SDL_Color{255,127,0,255}
                                : SDL_Color{192,192,192,255};
        SDL_Color colorScores = stateScores ? SDL_Color{255,127,0,255}
                                : SDL_Color{192,192,192,255};
        SDL_Color colorExit = stateExit ? SDL_Color{255,127,0,255}
                                : SDL_Color{192,192,192,255};
        

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

        SDL_RenderFillRect(renderer, &box);
        SDL_SetRenderDrawColor(renderer, colorScores.r, colorScores.g, colorScores.b, 255);
        SDL_RenderFillRect(renderer, &boxScores);
        SDL_SetRenderDrawColor(renderer, colorExit.r, colorExit.g, colorExit.b, 255);
        SDL_RenderFillRect(renderer, &boxExit);

        // ---- render main text ----
        if (textTexture)
        {
            int w, h;
            SDL_QueryTexture(textTexture, NULL, NULL, &w, &h);

            SDL_Rect r = {
                box.x + (box.w - w) / 2,
                box.y + (box.h - h) / 2,
                w, h
            };

            SDL_RenderCopy(renderer, textTexture, NULL, &r);
        }

        // ---- render scores text ----
        if (scoresTexture)
        {
            int w, h;
            SDL_QueryTexture(scoresTexture, NULL, NULL, &w, &h);

            SDL_Rect r = {
                boxScores.x + (boxScores.w - w) / 2,
                boxScores.y + (boxScores.h - h) / 2,
                w, h
            };

            SDL_RenderCopy(renderer, scoresTexture, NULL, &r);
        }

        // ---- render exit text ----
        if (exitTexture)
        {
            int w, h;
            SDL_QueryTexture(exitTexture, NULL, NULL, &w, &h);

            SDL_Rect r = {
                boxExit.x + (boxExit.w - w) / 2,
                boxExit.y + (boxExit.h - h) / 2,
                w, h
            };

            SDL_RenderCopy(renderer, exitTexture, NULL, &r);
        }
    }

    void stateHolder(SDL_Renderer* renderer, TTF_Font* font, std::string text, bool stat, bool scores, bool exit)
    {
        state = stat;
        stateScores = scores;
        stateExit=exit;
        box_Initializer(renderer, font, text);
    }
};


/*---------------------Character------------------------*/



class player : public hitBox
{
public:
    SDL_Rect character ;
    float jumpVelocity = 0.0f;
    float gravity = 1800.0f;
    float jumpPower = -950.0f;
    bool onGround = false;
    bool dashcooldown=false;
    bool dashstop=false;
    float ofsset=100.0f;
    
    float returnSpeed=-20.0f;
    float fallOffest=0;

    player(int sx=150,int sy=100,int sw=50,int sh=50):hitBox(sx,sy,sw,sh)
    {

        character={x,y,w,h};
    }

    void syncHitBox()
    {
        x = character.x;
        y = character.y;
        w = character.w;
        h = character.h;
    }

    void newPosition(int nx, int ny, int nw, int nh)
    {
        character = {nx, ny, nw, nh};
        syncHitBox();
    }

    void Render(SDL_Renderer* renderer)
    {
        // Set color (R, G, B, A)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Draw filled rectangle
        SDL_RenderFillRect(renderer, &character);
    }
};
