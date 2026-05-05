#include "object.h"

GameOver::GameOver()
{
    reset();
}

void GameOver::reset()
{
    active = false;
    deathCause = DeathCause::None;
    savedFinalScore = 0;
}

void GameOver::trigger(DeathCause cause, int score)
{
    active = true;
    deathCause = cause;
    savedFinalScore = score;
}

bool GameOver::isActive() const
{
    return active;
}

DeathCause GameOver::cause() const
{
    return deathCause;
}

int GameOver::finalScore() const
{
    return savedFinalScore;
}

std::string GameOver::causeText() const
{
    switch (deathCause)
    {
        case DeathCause::FellOff:
            return "Fell off";
        case DeathCause::HitTrap:
            return "Hit a trap";
        case DeathCause::None:
        default:
            return "Unknown";
    }
}
