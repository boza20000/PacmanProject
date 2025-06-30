#include "RedGhost.h"

// Activate the Red Ghost (Blinky)
void RedGhost::activateGhost() const
{
	shortestPathAlgorithm();  // Call the function to move the Red Ghost based on the shortest path algorithm
}

// Moves the Red Ghost (Blinky) backwards to its previous position if the move is possible
void RedGhost::moveBackwards(const char* color) const
{
	int curX = getGhostXPosition(blinkyNumber);  // Get the current X position of Blinky
	int curY = getGhostYPosition(blinkyNumber);  // Get the current Y position of Blinky
	if (isBackwardsPossible(redLastX, redLastY)) {  // If the position is valid for moving backwards
		moveGhostTo(redLastX, redLastY, blinkyNumber, blinkySymbol, color);  // Move Blinky backwards
		redLastX = curX;  // Update Blinky's last known X position
		redLastY = curY;  // Update Blinky's last known Y position
	}
}

void RedGhost::shortestPathAlgorithm() const
{
	int ghostCurX = getGhostXPosition(blinkyNumber);
	int ghostCurY = getGhostYPosition(blinkyNumber);
	int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };
	checkDirectionsAvailability(ghostCurX, ghostCurY, distance, 0, 0, redLastX, redLastY);//checks available directions for the ghost
	//changes to the available position or stays at the same place and waits
	ghostChangePosition(ghostCurX, ghostCurY, distance, blinkyNumber, blinkySymbol, redColor, redLastX, redLastY);
}
