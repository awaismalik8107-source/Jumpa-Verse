#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <cctype>

inline std::string toUpperUsername(const std::string& username)
{
    std::string upperUsername = username;
    for (char& character : upperUsername)
    {
        character = (char)std::toupper((unsigned char)character);
    }
    return upperUsername;
}

extern bool exitb;
extern float cammeraDash;
extern std::string currentPlayerUsername;
class ground_Class;
class trapSpike;
class player;
class Score;
class GameOver;
enum class GameOverAction;
enum class DeathCause;
enum class LoginScreenAction;
enum class RegisterScreenAction;
bool init(SDL_Window*& Window, SDL_Renderer*& renderer);
void close(SDL_Window* window, SDL_Renderer* renderer);

std::vector<ground_Class> ground_generator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex);


// ground_Class groundRandomgenerator(SDL_Renderer* renderer,SDL_Texture* groundTex,SDL_Texture* ugTex,std::vector<ground_Class> &prevGround,std::vector<trapSpike>& trap);

void groundRandomgenerator(SDL_Renderer* renderer,
                           SDL_Texture* groundTex,
                           SDL_Texture* ugTex,
                           std::vector<ground_Class> &prevGround,
                           std::vector<trapSpike>& trap);
// std::vector<ground_Class> cameraMovmentObj(std::vector<ground_Class> ground,float cameraOffset);

// std::vector<ground_Class> freeMemory(std::vector<ground_Class> ground);
void freeMemory(std::vector<ground_Class>& ground);
void cameraMovmentObj(std::vector<ground_Class>& ground, float cameraOffset,std::vector<trapSpike>&trap);


void trapTriangle(SDL_Renderer* renderer, std::vector<trapSpike>& trap, std::vector<ground_Class>& ground, int i, int maxTraps = 4);
void trapFreeMemory(std::vector<trapSpike>&trap);

bool menuScreen(SDL_Renderer* renderer, TTF_Font* font);
LoginScreenAction runLoginScreen(SDL_Renderer* renderer, TTF_Font* font);
RegisterScreenAction runRegisterScreen(SDL_Renderer* renderer, TTF_Font* font);
void runScoreMenu(SDL_Renderer* renderer, TTF_Font* font);

void physicsDevelop(player& player1,
                    std::vector<ground_Class>& g1,
                    float deltaTime,
                    const Uint8* keyboardState);

void initializeScore(Score& score);
void updateScore(Score& score, float deltaTime, bool playerAlive);
void renderScore(Score& score, SDL_Renderer* renderer, TTF_Font* font);
void resetScore(Score& score);

bool playerFellOff(const player& player1, int fallY = 1200);
bool playerHitTrap(const player& player1, const std::vector<trapSpike>& trap);
GameOverAction runGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, GameOver& gameOver);
void saveFinalScore(const GameOver& gameOver, const std::string& path = "score.csv");
void saveLoggedInPlayerScore(const GameOver& gameOver, const std::string& path = "player.csv");
