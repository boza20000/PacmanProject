
#pragma once
#include "Ghost.h"
#include "Settings.h"
#include <climits>
#include <iostream>

class PinkGhost : public Ghost
{
public:

	PinkGhost(const char* color, Position position, char symbol, size_t scoreToActivate, size_t ghostNumber) :
		Ghost(color, position, symbol, scoreToActivate, ghostNumber) {
	}
	// Moves the Pink Ghost (Pinky) backwards to its previous position if the move is possible
	void moveBackwards(const char* color)const override;
	// Activate the Pink Ghost (Pinky)
	 void activateGhost()const override;
	/// Pink Ghost algorithm
	 void shortestPathAlgorithm() const override;


};
