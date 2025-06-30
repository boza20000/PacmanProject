#pragma once
#include "Ghost.h"
#include "Settings.h"
#include <iostream>
#include <climits>

class RedGhost : public Ghost
{
public:
	RedGhost(const char* color, Position position, char symbol, size_t scoreToActivate, size_t ghostNumber) :
		Ghost(color, position, symbol, scoreToActivate, ghostNumber) {}
	// Moves the Red Ghost (Blinky) backwards to its previous position if the move is possible
	void moveBackwards(const char* color)const override;
	//red algorithm
	void shortestPathAlgorithm() const override;
	// Activate the Red Ghost (Blinky)
	 void activateGhost()const override;
};
