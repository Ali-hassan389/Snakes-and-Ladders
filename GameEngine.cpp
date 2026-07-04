
#include "GameEngine.h"

GameEngine::GameEngine(const vector<string>& names, const vector<int>& colorIds) {
    for (int i = 0; i < names.size(); ++i) {
        players.emplace_back(names[i], colorIds[i]);
    }
    currentPlayerIndex = 0;
    turnCount = 1;
    gameOver = false;

    lastOldPos = 0;
    lastLandedPos = 0;
    lastFinalPos = 0;
    lastMoveValid = true;
    hitExtraTurn = false;
}

GameEngine::~GameEngine() {}

int GameEngine::rollDice() {
    return dice.roll();
}

void GameEngine::applyMove(int roll) {
    if (gameOver)
        return;

    Player& activePlayer = players[currentPlayerIndex];
    lastOldPos = activePlayer.getPosition();

    if (lastOldPos + roll > 100) {
        lastLandedPos = lastOldPos;
        lastFinalPos = lastOldPos;
        lastMoveValid = false;
        hitExtraTurn = false;
        return;
    }

    lastMoveValid = true;
    lastLandedPos = lastOldPos + roll;

    lastFinalPos = board.getDestination(lastLandedPos);
    activePlayer.setPosition(lastFinalPos);

    if (lastFinalPos == 100) {
        gameOver = true;
        hitExtraTurn = false;
        return;
    }

    if (roll == 6) {
        hitExtraTurn = true;
    }
    else {
        hitExtraTurn = false;
    }
}

void GameEngine::nextTurn() {
    if (gameOver) return;

    if (hitExtraTurn) {
        hitExtraTurn = false;
        turnCount++;
        return;
    }

    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    turnCount++;
}

int GameEngine::getWinnerIndex() const {
    for (int i = 0; i < players.size(); ++i) {
        if (players[i].getPosition() == 100) {
            return i;
        }
    }
    return 0;
}