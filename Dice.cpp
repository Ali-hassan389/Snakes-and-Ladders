#include "Dice.h"

//Constructor so random number everytime program runs
Dice::Dice() {
	srand((unsigned)time(0));
 }
//returns a random number 1-6
int Dice::roll() {
	lastRoll= (rand() % 6) + 1;
	return lastRoll;
}

//returns the last dice rolled
int Dice::getLastRoll() const {
	return lastRoll;
}
