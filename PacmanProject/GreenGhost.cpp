#include "GreenGhost.h"
#include "Pacman.h"


// Moves the Green Ghost (Clyde) backwards to its previous position if the move is possible
void GreenGhost::moveBackwards(const char* color) const
{
	int curX = getGhostXPosition(clydeNumber);  // Get the current X position of Clyde
	int curY = getGhostYPosition(clydeNumber);  // Get the current Y position of Clyde
	if (isBackwardsPossible(greenLastX, greenLastY)) {  // If the position is valid for moving backwards
		moveGhostTo(greenLastX, greenLastY, clydeNumber, clydeSymbol, color);  // Move Clyde backwards
		greenLastX = curX;  // Update Clyde's last known X position
		greenLastY = curY;  // Update Clyde's last known Y position
	}
}

// Activate the Green Ghost (Clyde)
void GreenGhost::activateGhost() const
{
	shortestPathAlgorithm();// Call the function to move the Green Ghost based on the shortest path algorithm
}

// Green Ghost algorithm (Clyde)
void GreenGhost::shortestPathAlgorithm() const
{
	// Get the current position of the Green ghost (Clyde)
	int ghostCurX = getGhostXPosition(clydeNumber);
	int ghostCurY = getGhostYPosition(clydeNumber);

	// If Clyde is less than 8 tiles away from Pacman, pursue Pacman
	if (Pacman::distanceToPacman(ghostCurX, ghostCurY, 0, 0) < 8) {
		// Create an array to store the distance for each direction (up, down, left, right)
		int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };

		// Check available directions and calculate distances for Clyde's move towards Pacman (target = (0, 0))
		checkDirectionsAvailability(ghostCurX, ghostCurY, distance, 0, 0, greenLastX, greenLastY);

		// Move Clyde based on the smallest distance (best direction towards Pacman)
		ghostChangePosition(ghostCurX, ghostCurY, distance, clydeNumber, clydeSymbol, greenColor, greenLastX, greenLastY);
	}
	// If Clyde is 8 or more tiles away from Pacman, switch to scatter mode (move towards the bottom-left corner)
	else if (Pacman::distanceToPacman(ghostCurX, ghostCurY, 0, 0) >= 8) {
		const int scatterTargetX = 2;  // Target X coordinate (bottom-left corner)
		const int scatterTargetY = heightGrid - 2;  // Target Y coordinate (bottom-left corner)

		// Create an array to store the distance for each direction (up, down, left, right)
		int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };

		// Check available directions and calculate distances for Clyde's move towards the scatter target
		checkDirectionsAvailability(ghostCurX, ghostCurY, distance, scatterTargetX, scatterTargetY, greenLastX, greenLastY);

		// Move Clyde based on the smallest distance (best direction towards the scatter target)
		ghostChangePosition(ghostCurX, ghostCurY, distance, clydeNumber, clydeSymbol, greenColor, greenLastX, greenLastY);

	}
}