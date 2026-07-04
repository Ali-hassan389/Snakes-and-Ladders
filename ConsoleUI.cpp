#include "ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <chrono>   
#include <thread>   

using namespace std;

// Sets up names and colors for all participating players.
 
void ConsoleUI::setupPlayers(vector<string>& names,vector<int>& colorIds) {
    int count = 0;

    // Simple while loop for input validation
    while (count < 2 || count > 6) {
        cout << "Enter number of players (2-6): ";
        cin >> count;
//If the user typed letters instead of a number
        if (cin.fail()) {
            cin.clear(); 
            string trash;
            cin >> trash; // Swallow the bad text typed by the user
        }
    }
    // A dummy read to clear the single 'Enter' character left behind by cin
    string dummy;
    getline(cin, dummy);

    // Loop to collect names based on the validated count
    for (int i = 0; i < count; i++) {
        string name;
        cout << "Enter name for Player " << (i + 1) << ": ";
        getline(cin, name);
     //Push values into arrays
        names.push_back(name);
        colorIds.push_back(i);
    }
}

//Display the entire 100-tile game board onto the console screen.
 
void ConsoleUI::printBoard() {
    cout << "\033[H"; // Clear screen helper: Moves cursor to top-left

    Board& board = engine->getBoard();
    auto& players = engine->getPlayers();

    cout << "=========================================================================\n";
    cout << "                            SNAKES & LADDERS                             \n";
    cout << "=========================================================================\n";

    // Standard nested loops to draw a 10x10 grid matrix
    for (int row = 9; row >= 0; row--) {
      cout << "| ";
        for (int col = 0; col < 10; col++) {
 // Basic logic to make tile numbers snake back and forth row by row
            int tile = 0;
            if (row % 2 == 0) {
                tile = row * 10 + col + 1;
            }
            else {
                tile = row * 10 + (9 - col) + 1;
            }

            bool playerPresent = false;
            int occupyingPlayerColorId = -1;
            char playerInitial = ' ';

            // Regular for loop checking if a player's position matches this tile
            for (auto& p : players) {
                if (p.getPosition() == tile) {
                    playerPresent = true;
                    occupyingPlayerColorId = p.getColorId();
                    playerInitial = p.getName()[0];
                    break;
                }
            }
cout << " ";
            // Drawing board elements using simple if/else branches
            if (board.isLadder(tile)) {
                cout << "\033[1;34mL" << left << setw(2) << tile << "\033[0m";
            }
            else if (board.isSnake(tile)) {
               cout << "\033[1;31mS" << left << setw(2) << tile << "\033[0m";
            }
            else {
                cout << "\033[90m" << left <<setw(3) << tile << "\033[0m";
            }

            // Print player character or an empty slot space
            if (playerPresent) {
                cout << "[\033[1m" << colorCodes[occupyingPlayerColorId]
                    << playerInitial << colorReset << "] ";
            }
            else {
                cout << "[ ] ";
            }
        }
        cout << "|\n";
    }
    cout << "=========================================================================\n";
}

// Core loop manager processing turns, rolling dice, and triggering animations.
 
void ConsoleUI::runGameLoop() {
    while (!engine->isGameOver()) {
        printBoard();

        Player& cur = engine->getCurrentPlayer();
        cout << "\033[K" << colorCodes[cur.getColorId()] << cur.getName() << colorReset
            << "'s turn - press Enter to roll!";

        // Simple Pause: Waits for the user to press Enter
        string waitRoll;
        getline(cin, waitRoll);

        // Basic animation loop using standard rand() math
        cout << "\033[KRolling: [ ]";
        for (int i = 0; i < 8; ++i) {
            int fakeRoll = (rand() % 6) + 1;
            cout << "\b\b" << fakeRoll << "]";
            cout.flush();
            this_thread::sleep_for(chrono::milliseconds(90));
        }

        int roll = engine->rollDice();
        cout << "\b\b\033[1;33m" << roll << "\033[0m] Final Result!\n";

        engine->applyMove(roll);
cout << "\033[K";
        if (!engine->wasLastMoveValid()) {
            cout << "Move skipped - would overshoot 100!\n";
        }
        else {
            cout << "Moved from " << engine->getLastOldPos()
                << " to " << engine->getLastLandedPos() << "\n";

            cout << "\033[K";
            if (engine->usedLadder()) {
               cout << "\033[1;34mLadder! Climbing up to " << engine->getLastFinalPos() << "\033[0m\n";
            }
            else if (engine->usedSnake()) {
               cout << "\033[1;31mSnake! Sliding down to " << engine->getLastFinalPos() << "\033[0m\n";
            }
            else {
                cout << "\n";
            }
        }

        cout << "\033[K";
        if (engine->hadExtraTurn()) {
            cout << colorCodes[cur.getColorId()] << cur.getName() << colorReset << " rolled a 6 - extra turn granted!\n";
            this_thread::sleep_for(chrono::seconds(2));
        }
        else {
            cout << "\n";
        }

        if (engine->hasWon()) break;

        cout << "Press Enter to hand over control...";
        string waitHandover;
        getline(cin, waitHandover);
        engine->nextTurn();
    }

    showWinnerScreen();

    vector<string> names;
    for (auto& p : engine->getPlayers()) names.push_back(p.getName());
    recordManager.saveRecord(names,
        engine->getPlayers()[engine->getWinnerIndex()].getName(),
        engine->getTurnCount(),
        "Console");
} 

// Displays the final celebration banner screen and waits for user confirmation.
void ConsoleUI::showWinnerScreen() {
    Player& winner = engine->getPlayers()[engine->getWinnerIndex()];

    cout << "\n************************************\n";
    cout << colorCodes[winner.getColorId()] << winner.getName() << colorReset
        << " WINS THE GAME!\n";
    cout << "Turns taken: " << engine->getTurnCount() << "\n";
   cout << "************************************\n";
   cout << "Press Enter to return to menu...";

    char ch = ' ';
    while (ch != '\n') {
        cin.get(ch);
    }
cout << "\033[2J\033[H";
}