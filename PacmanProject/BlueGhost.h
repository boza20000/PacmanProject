#pragma once
#include "Ghost.h"
#include "Settings.h"
#include <iostream>
#include <climits>
class BlueGhost : public Ghost
{
public:
	BlueGhost(const char * color, Position position, char symbol, size_t scoreToActivate, size_t ghostNumber) :
		Ghost(color, position, symbol, scoreToActivate, ghostNumber) {
	}

	// Moves the Blue Ghost (Inky) backwards to its previous position if the move is possible
	void moveBackwards(const char* color)const override;

	// Activate the Blue Ghost (Inky)
	void activateGhost() const override;

	// Blue Ghost algorithm
	void shortestPathAlgorithm() const override;
};
