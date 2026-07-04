#pragma once

#include "GameEngine.h"
#include "RecordManager.h"
#include <string>
#include <vector>

using namespace std;

class ConsoleUI {
private:
    // text coloring configurations for players
    const string colorCodes[6] = {
        "\033[1;35m", // Magenta
        "\033[1;33m", // Yellow
        "\033[1;36m", // Cyan
        "\033[1;32m", // Green
        "\033[1;37m", // White
        "\033[1;95m"  // Light Pink
    };
    const string colorReset = "\033[0m";
public:
    GameEngine* engine;
    RecordManager recordManager;
    void setupPlayers(vector<string>& names,vector<int>& colorIds);
    void printBoard();
    void runGameLoop();
    void showWinnerScreen();
    ConsoleUI() : engine(nullptr), recordManager("data/records.txt") {}
    ~ConsoleUI() { 
        if (engine) 
            delete engine;
    }

   
};