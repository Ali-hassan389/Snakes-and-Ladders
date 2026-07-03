#pragma once
#include<iostream>
#include<cstdlib>
#include<ctime>
using namespace std;

//Dice class for random numbers 1-6
class Dice {
	int lastRoll;
public:
	Dice();
	int roll();
	int getLastRoll()const;
};


