#include "Board.h"

Board::Board() {}

// Converts a 1-100 tile number into 2D row/col coordinates.
// Accounts for the alternating left-to-right / right-to-left snake pattern.
void Board::tileToRowCol(int tile, int& row, int& col) const {
    row = (tile - 1) / 10;
    if (row % 2 == 0) {
        col = (tile - 1) % 10;
    }
    else {
        col = 9 - ((tile - 1) % 10);
    }
}
// Checks if a tile lands on a snake/ladder and returns the destination tile.
int Board::getDestination(int tile) const {
    for (int i = 0; i < NUM_LADDERS; i++) {
        if (ladderStart[i] == tile)
            return ladderEnd[i];
    }

    for (int i = 0; i < NUM_SNAKES; i++) {
        if (snakeStart[i] == tile)
            return snakeEnd[i];
    }

    return tile;
}
// Returns true if the tile contains the start of a ladder.
bool Board::isLadder(int tile) const {
    for (int i = 0; i < NUM_LADDERS; i++) {
        if (ladderStart[i] == tile)
            return true;
    }
    return false;
}
// Returns true if the tile contains the head of a snake.
bool Board::isSnake(int tile) const {
    for (int i = 0; i < NUM_SNAKES; i++) {
        if (snakeStart[i] == tile)
            return true;
    }
    return false;
}
// Checks if the tile number is within the valid board range (1 to 100).
bool Board::isValidTile(int tile) const {
    return (tile >= 1 && tile <= 100);
}