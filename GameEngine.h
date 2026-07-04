#pragma once
#include "Board.h"
#include "Player.h"
#include "Dice.h"
#include <string>
#include <vector>
using namespace std;


class GameEngine {
private:
    Board board;
    Dice dice;
    //used vectors to expand or shrink current player size
    vector<Player> players;

    size_t currentPlayerIndex;
    int turnCount;
    bool gameOver;

    int lastOldPos;
    int lastLandedPos;
    int lastFinalPos;

    bool lastMoveValid;
    bool hitExtraTurn;

public:
    GameEngine(const vector<string>& names,const vector<int>&colorIds);
    ~GameEngine();

    Board& getBoard() { return board; }
    vector<Player>& getPlayers() { return players; }
    Player& getCurrentPlayer() { return players[currentPlayerIndex]; }

    int rollDice();
    void applyMove(int roll);
    void nextTurn();

    bool isGameOver() const { return gameOver; }
    bool hasWon() const { return gameOver; }

    // Simple Getter Functions
    int getWinnerIndex() const;
    int getTurnCount() const { return turnCount; }
    int getLastOldPos() const { return lastOldPos; }
    int getLastLandedPos() const { return lastLandedPos; }
    int getLastFinalPos() const { return lastFinalPos; }
    bool wasLastMoveValid() const { return lastMoveValid; }
    bool usedLadder() const { return lastFinalPos > lastLandedPos; }
    bool usedSnake() const { return lastFinalPos < lastLandedPos; }
    bool hadExtraTurn() const { return hitExtraTurn; }
};