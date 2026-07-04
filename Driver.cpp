#include "ConsoleUI.h"
#include "SfmlUI.h" 
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    ConsoleUI consoleUI;
    while (true) {
        cout << "=================================\n";
        cout << "   SNAKES AND LADDERS LAUNCHER   \n";
        cout << "=================================\n";
        cout << "1. Console Mode\n"
            << "2. SFML Mode\n"
            << "3. Records\n"
            << "4. Exit\n"
            << "Enter choice: ";
        int choice;
        cin >> choice;
        // If the user typed a letter instead of a number
        if (cin.fail()) {
            cin.clear(); 
            // Loop and grab individual bad characters until we clear the whole line
            char trash = ' ';
            while (trash != '\n') {
                cin.get(trash);
            }
            continue;
        }
        // OPTION 1: CONSOLE GAMEPLAY
        if (choice == 1) {
            vector<string> names;
            vector<int> colorIds;
            consoleUI.setupPlayers(names, colorIds);
            consoleUI.engine = new GameEngine(names, colorIds);
            cout << "\033[2J\033[H";
            consoleUI.runGameLoop();
            delete consoleUI.engine;
            consoleUI.engine = nullptr;
        }
        // OPTION 2: SFML GRAPHICAL WINDOW
        else if (choice == 2) {
            vector<string> names;
            vector<int> colorIds;
            consoleUI.setupPlayers(names, colorIds); 
            cout << "Launching SFML Window...\n";
            SfmlUI sfmlApp;
            sfmlApp.runGame(names, colorIds);
            cout << "\033[2J\033[H";
        }
        // OPTION 3: PRINT RECORDS
        else if (choice == 3) {
            consoleUI.recordManager.printAllRecords();
        } 
        // OPTION 4: EXIT
        else if (choice == 4) {
            break;
        }
    }

    return 0;
}