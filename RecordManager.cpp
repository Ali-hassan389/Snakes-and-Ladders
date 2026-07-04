#include "RecordManager.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

RecordManager::RecordManager(string path) : filePath(path) {
}

void RecordManager::saveRecord(const vector<string>& playerNames,const string& winnerName,
    int turnCount,const string& mode) {

    // Open the file in append mode.
    ofstream outFile(filePath,ios::app);
    if (!outFile) {
        cout << "[ERROR]: Could not open or create " << filePath << " to write record!\n";
        return;
    }

    //Write players into a comma-separated list
    for (int i = 0; i < playerNames.size(); ++i) {
        outFile << playerNames[i];
        if (i < playerNames.size() - 1) {
            outFile << ",";
        }
    }

    //Write other data using | delimiter
    outFile << " | " << winnerName << " | " << turnCount<< " | " << mode << "\n";
    outFile.close(); 
}

//Reads and prints all saved match records in a simple table

void RecordManager::printAllRecords() const {
   ifstream inFile(filePath);
    //If the file doesn't exist yet, show a clean message
    if (!inFile) {
        cout << "\n=========================================\n";
        cout << "          PAST GAME MATCH HISTORY         \n";
        cout << "=========================================\n";
        cout << " No match records found. Play a game first!\n";
        cout << "=========================================\n";
        return;
    }

    // Print out the table headers
    cout << "\n=========================================================================\n";
    cout << "                         PAST GAME MATCH HISTORY                         \n";
    cout << "=========================================================================\n";
    cout << left << setw(30) << "Players"<<setw(15) << "Winner"
         <<setw(12) << "Total Turns"<<setw(10) << "Mode" << "\n";
    cout << "-------------------------------------------------------------------------\n";

    string players, winner, turns, mode;
    // Read directly from the file using the pipe '|'
    while (getline(inFile, players, '|') && getline(inFile, winner, '|') &&
        getline(inFile, turns, '|') && getline(inFile, mode)) {
        cout <<left<< setw(30) << players<< setw(15) << winner
             << setw(12) << turns<< setw(10) << mode << "\n";
    }
cout << "=========================================================================\n";
    inFile.close();
}