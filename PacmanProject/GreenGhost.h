#pragma once
#include "Ghost.h"
#include "Settings.h"
#include <iostream>
#include <climits>
class GreenGhost : public Ghost
{
public:
	GreenGhost(const char* color, Position position, char symbol, size_t scoreToActivate, size_t ghostNumber) :
		Ghost(color, position, symbol, scoreToActivate, ghostNumber) {
	}

	// Moves the Green Ghost (Clyde) backwards to its previous position if the move is possible
	void moveBackwards(const char* color)const override;

	// Activate the Green Ghost (Clyde)
	void activateGhost() const override;

	// Green Ghost algorithm (Clyde)
	void shortestPathAlgorithm() const override;

};


