#pragma once
class Board {
private:
    //Number of ladders and snakes on the board
    static const int NUM_LADDERS = 6;
    static const int NUM_SNAKES = 8;

    //positions of ladder start and ladder end
    const int ladderStart[NUM_LADDERS] = { 2, 8, 20, 32, 50, 71 };
    const int ladderEnd[NUM_LADDERS] = { 23, 34, 77, 68, 91, 92 };

    //position of snakes start and snake end
    const int snakeStart[NUM_SNAKES] = { 17, 54, 62, 64, 87, 93, 95, 98 };
    const int snakeEnd[NUM_SNAKES] = { 4, 19, 18, 60, 24, 73, 75, 79 };

public:
    Board();

    void tileToRowCol(int tile, int& row, int& col) const;
    int getDestination(int tile) const;

    bool isLadder(int tile) const;
    bool isSnake(int tile) const;

    bool isValidTile(int tile) const;
};