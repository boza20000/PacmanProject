#pragma once
//Solution to course project # <5>
//Introduction to programming course
//Faculty of Mathematics and Informatics od Sofia University
//Winter semester 2024/2025
//
//@author <Boris Dimitrov Tsvetkov>
//idNumber <6MI0600504
//@compiler Microsoft Visual Studio compiler
//header file with helper variables

const size_t amountOfGhosts = 4;
const size_t foodAmount = 4;
const size_t prizeOfFood = 20;
const size_t prizeOfPoint = 1;
int ghostX[amountOfGhosts], ghostY[amountOfGhosts];
int foodX[foodAmount], foodY[foodAmount];
char lastSymbol[amountOfGhosts] = { ' ',' ',' ',' ' };
const size_t wallsIncl = 2;
const char* blueColor = "\033[34m";
const char* endColor = "\033[0m";
size_t collectedAmountOfFood = 0;
size_t frightenedModeCount = 0;
bool isChaseMode = true;
bool isFrightenedMode = false;
bool cantMove = false;

