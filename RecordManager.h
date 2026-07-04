#pragma once
#include <string>
#include <vector>
using namespace std;

class RecordManager {
private:
    string filePath;
public:
    // Constructor to enter filepath
    RecordManager(string path = "data/records.txt");

    void saveRecord(const vector<string>&playerNames,const string& winnerName,
        int turnCount,const string& mode);

    void printAllRecords() const;
};