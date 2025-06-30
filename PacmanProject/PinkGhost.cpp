#include "PinkGhost.h"

// Moves the Pink Ghost (Pinky) backwards to its previous position if the move is possible
void PinkGhost::moveBackwards(const char* color) const
{
	int curX = getGhostXPosition(pinkyNumber);  // Get the current X position of Pinky
	int curY = getGhostYPosition(pinkyNumber);  // Get the current Y position of Pinky
	if (isBackwardsPossible(pinkLastX, pinkLastY)) {  // If the position is valid for moving backwards
		moveGhostTo(pinkLastX, pinkLastY, pinkyNumber, pinkySymbol, color);  // Move Pinky backwards
		pinkLastX = curX;  // Update Pinky's last known X position
		pinkLastY = curY;  // Update Pinky's last known Y position
	}
}

// Activate the Pink Ghost (Pinky)
void PinkGhost::activateGhost() const
{
	shortestPathAlgorithm();  // Call the function to move the Pink Ghost based on the shortest path algorithm
}
/// Pink Ghost algorithm
void PinkGhost::shortestPathAlgorithm() const
{
	// Get the current position of the Pinky ghost
	int ghostCurX = getGhostXPosition(pinkyNumber);
	int ghostCurY = getGhostYPosition(pinkyNumber);

	// Initialize variables for determining the direction of Pacman and the target offset for Pinky
	int changeOfPacmanNumberY = 0;
	int changeOfPacmanNumberX = 0;
	int targetOffset = 4;  // Set the target offset value (Pinky tries to go ahead of Pacman)

	// Adjust the target position based on Pacman's current movement direction
	if (isGoingRight) {
		changeOfPacmanNumberX += targetOffset;  // Move Pinky to the right of Pacman
	}
	else if (isGoingLeft) {
		changeOfPacmanNumberX -= targetOffset;  // Move Pinky to the left of Pacman
	}
	if (isGoingDown) {
		changeOfPacmanNumberY += targetOffset;  // Move Pinky below Pacman
	}
	else if (isGoingUp) {
		changeOfPacmanNumberY -= targetOffset;  // Move Pinky above Pacman
		changeOfPacmanNumberX -= targetOffset;  // Move Pinky to the left if Pacman is moving up
	}

	// Create an array to store the distance for each direction (up, down, left, right)
	int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };

	// Check available directions and calculate distances considering Pinky's target position
	checkDirectionsAvailability(ghostCurX, ghostCurY, distance, changeOfPacmanNumberX, changeOfPacmanNumberY, pinkLastX, pinkLastY);

	// Move Pinky based on the smallest distance (best direction)
	ghostChangePosition(ghostCurX, ghostCurY, distance, pinkyNumber, pinkySymbol, pinkColor, pinkLastX, pinkLastY);
}



