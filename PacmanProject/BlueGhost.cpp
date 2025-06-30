#include "BlueGhost.h"

// Moves the Blue Ghost (Inky) backwards to its previous position if the move is possible
void BlueGhost::moveBackwards(const char* color) const
{
	int curX = getGhostXPosition(inkyNumber);  // Get the current X position of Inky
	int curY = getGhostYPosition(inkyNumber);  // Get the current Y position of Inky
	if (isBackwardsPossible(blueLastX, blueLastY)) {  // If the position is valid for moving backwards
		moveGhostTo(blueLastX, blueLastY, inkyNumber, inkySymbol, color);  // Move Inky backwards
		blueLastX = curX;  // Update Inky's last known X position
		blueLastY = curY;  // Update Inky's last known Y position
	}
}

// Activate the Blue Ghost (Inky)
void BlueGhost::activateGhost() const
{
	shortestPathAlgorithm();// Call the function to move the Blue Ghost based on the shortest path algorithm
}

// Blue Ghost algorithm
void BlueGhost::shortestPathAlgorithm() const
{
	// Get the current position of the Blue ghost (Inky) and the Red ghost (Blinky)
	int ghostCurX = getGhostXPosition(inkyNumber);
	int ghostCurY = getGhostYPosition(inkyNumber);
	int redCurX = getGhostXPosition(blinkyNumber);
	int redCurY = getGhostYPosition(blinkyNumber);

	// Set the initial target position to be Pacman's current position
	int changeOfPacmanX = pacmanX;
	int changeOfPacmanY = pacmanY;

	// Adjust the target position based on Pacman's current movement direction
	if (isGoingUp) {
		changeOfPacmanY -= 2;  // Move Inky upwards relative to Pacman
	}
	else if (isGoingDown) {
		changeOfPacmanY += 2;  // Move Inky downwards relative to Pacman
	}
	else if (isGoingLeft) {
		changeOfPacmanX -= 2;  // Move Inky to the left relative to Pacman
	}
	else if (isGoingRight) {
		changeOfPacmanX += 2;  // Move Inky to the right relative to Pacman
	}

	// Calculate the direction vector between Red ghost and Pacman's target position
	int vectorX = changeOfPacmanX - redCurX;
	int vectorY = changeOfPacmanY - redCurY;

	// Inky's target is twice the vector distance from Red ghost, creating a shadow effect
	int inkyTargetX = redCurX + 2 * vectorX;
	int inkyTargetY = redCurY + 2 * vectorY;

	// Create an array to store the distance for each direction (up, down, left, right)
	int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };

	// Check available directions and calculate distances considering Inky's target position
	checkDirectionsAvailability(ghostCurX, ghostCurY, distance, inkyTargetX, inkyTargetY, blueLastX, blueLastY);

	// Move Inky based on the smallest distance (best direction)
	ghostChangePosition(ghostCurX, ghostCurY, distance, inkyNumber, inkySymbol, cyanColor, blueLastX, blueLastY);
}
