#pragma once
#include <string>
using namespace std;

//Player ID and current position class 
class Player {
private:
    string name;
    int colorId;
    int position;

public:
    Player();
    Player(string n, int colorID);

    int getPosition() const;
    void setPosition(int p);

    string getName() const;
    int getColorId() const;
};