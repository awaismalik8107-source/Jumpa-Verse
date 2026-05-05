#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <SDL2/SDL_ttf.h>
#include <string>
extern bool exitb;
extern float cammeraDash;
class ground_Class;
class trapSpike;
class player;
class Score;
class GameOver;
enum class GameOverAction;
enum class DeathCause;
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
