#pragma once
#include "GreenGhost.h"
#include "BlueGhost.h"
#include "PinkGhost.h"
#include "RedGhost.h"
extern const size_t wallsIncl;
extern const char pacmanSymbol;
extern const char* yellowColor;
extern int pacmanX, pacmanY;
extern size_t playerScore;
extern bool isGoingLeft;
extern bool isGoingRight;
extern bool isGoingUp;
extern bool isGoingDown;
extern const char blinkySymbol;
extern const size_t blinkyNumber;
extern const char* redColor;
extern const size_t scoreToActivtRed;
extern int redLastX;
extern int redLastY;
extern const char pinkySymbol;
extern const size_t pinkyNumber;
extern const char* pinkColor;
extern const size_t scoreToActivtPink;
extern int pinkLastX;
extern int pinkLastY;
extern const char wallSymbol;
extern const char foodSymbol;
extern const char pointSymbol;
extern const char emptySymbol;
extern char** grid;
extern int widthGrid, heightGrid;
extern const char clydeSymbol;
extern const size_t clydeNumber;
extern const char* greenColor;
extern const size_t scoreToActivtGreen;
extern int greenLastX;
extern int greenLastY;
extern const size_t amountOfGhosts;
extern const size_t foodAmount;
extern const size_t prizeOfFood;
extern const size_t prizeOfPoint;
extern int ghostX[], ghostY[];
extern int foodX[], foodY[];
extern char lastSymbol[];
extern const char* blueColor;
extern const char* endColor;
extern size_t collectedAmountOfFood;
extern size_t frightenedModeCount;
extern bool isChaseMode;
extern bool isFrightenedMode;
extern bool cantMove;
extern const char inkySymbol;
extern const size_t inkyNumber;
extern const char* cyanColor;
extern const size_t scoreToActivtBlue;
extern int blueLastX;
extern int blueLastY;
extern  Ghost* redGhost;
extern  Ghost* pinkGhost;
extern  Ghost* blueGhost;
extern  Ghost* greenGhost;

