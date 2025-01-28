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


const char pacmanSymbol = 'P';
const char* yellowColor = "\033[33m";
int pacmanX, pacmanY;
size_t playerScore = 0;
bool isGoingLeft = false;
bool isGoingRight = false;
bool isGoingUp = false;
bool isGoingDown = false;

//void spawnPacman() ;
//void erasePacmanFromOldPosition() ;
//void updatePacmanGridPosition() ;
//void clearPacmanPreviousCell();
//void setPacmanPosition() ;
//void movePacmanInDirection(int movingX, int movingY, char nextSymbol);
//void movePacmanUp();
//void movePacmanDown();
//void movePacmanLeft() ;
//void movePacmanRight();
//void handlePacmanMovement();
//void updateScore(int points);