#include "Player.h"

//Default constructor for default values
Player::Player() {
    name = "Unknown";
    colorId = 0;
    position = 0;
}

//Parameterized constructor 
Player::Player(string n, int colorID) : name(n), colorId(colorID) {
    position = 0;
}

//Getters and Setters
int Player::getPosition() const {
    return position;
}
void Player::setPosition(int p) {
    position = p;
}
string Player::getName() const {
    return name;
}

int Player::getColorId() const {
    return colorId;
}