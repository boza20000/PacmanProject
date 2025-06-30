#include "Ghost.h"
#include "Console.h"
#include "Settings.h"
#include <iostream>
#include <cstdlib>
#include "Pacman.h"
#include "RedGhost.h"
#include "PinkGhost.h"
#include "BlueGhost.h"
#include "GreenGhost.h"

// Function to get the Y position of a specific ghost
int Ghost::getGhostYPosition(int ghostNumber) {
	return ghostY[ghostNumber];  // Return the Y coordinate of the specified ghost
}

// Function to get the X position of a specific ghost
int Ghost::getGhostXPosition(int ghostNumber) {
	return ghostX[ghostNumber];  // Return the X coordinate of the specified ghost
}

// Function to set the position of a specific ghost
void Ghost::setGhostPosition(int x, int y, int ghostNumber) {
	ghostX[ghostNumber] = x;  // Set X position of the ghost
	ghostY[ghostNumber] = y;  // Set Y position of the ghost
}

// Function to repaint a ghost after moving it, changing its color
void Ghost::repaintGhost(int ghostNumber, const char* color) {
	int x = getGhostXPosition(ghostNumber);  // Get the current X position of the ghost
	int y = getGhostYPosition(ghostNumber);  // Get the current Y position of the ghost

	Console::setConsoleCursorPosition(x * 2, y);  // Position the cursor for the old ghost position
	std::cout << emptySymbol;  // Clear the old position of the ghost

	Console::setConsoleCursorPosition(x * 2, y);  // Position the cursor to repaint the ghost
	std::cout << color << grid[y][x] << endColor;  // Repaint the ghost with the provided color
}

// Function to spawn all ghosts at specific initial positions
void Ghost::spawnGhost() {
	// Set initial positions of all ghosts
	setGhostPosition(1, 2, blinkyNumber);  // Blinky
	setGhostPosition(1, 1, pinkyNumber);  // Pinky
	setGhostPosition(2, 1, inkyNumber);   // Inky
	setGhostPosition(3, 1, clydeNumber);  // Clyde

	// Save the symbols where the ghosts initially spawn
	lastSymbol[blinkyNumber] = grid[2][1];
	lastSymbol[pinkyNumber] = grid[1][1];
	lastSymbol[inkyNumber] = grid[1][2];
	lastSymbol[clydeNumber] = grid[1][3];

	// Place ghosts on the grid
	grid[ghostY[0]][ghostX[0]] = blinkySymbol;
	grid[ghostY[1]][ghostX[1]] = pinkySymbol;
	grid[ghostY[2]][ghostX[2]] = inkySymbol;
	grid[ghostY[3]][ghostX[3]] = clydeSymbol;

	// Repaint ghosts with their respective colors
	repaintGhost(blinkyNumber, redColor);  // Red for Blinky
	repaintGhost(pinkyNumber, pinkColor);  // Pink for Pinky
	repaintGhost(inkyNumber, cyanColor);  // Cyan for Inky
	repaintGhost(clydeNumber, greenColor);  // Green for Clyde
}

// Function to erase a ghost from its old position when it moves
void Ghost::eraseGhostFromOldPosition(int ghostNumber) {
	int x = getGhostXPosition(ghostNumber);  // Get X position of the ghost
	int y = getGhostYPosition(ghostNumber);  // Get Y position of the ghost
	char previousSymbol = lastSymbol[ghostNumber];  // Get the last symbol at the ghost's position

	// Restore the previous symbol (food, point, or empty) in the grid at the ghost's old position
	if (previousSymbol == foodSymbol) {
		grid[y][x] = foodSymbol;
		Console::setConsoleCursorPosition(x * 2, y);  // Position the cursor to the old ghost position
		std::cout << yellowColor << grid[y][x] << endColor;  // Repaint food
	}
	else if (previousSymbol == pointSymbol) {
		grid[y][x] = pointSymbol;
		Console::setConsoleCursorPosition(x * 2, y);
		std::cout << grid[y][x];  // Repaint point
	}
	else if (previousSymbol == emptySymbol) {
		grid[y][x] = emptySymbol;
		Console::setConsoleCursorPosition(x * 2, y);
		std::cout << grid[y][x];  // Repaint empty space
	}
}

// Function to move a ghost to a new position and repaint it with its color
void Ghost::moveGhostTo(int x, int y, int ghostNumber, char symbol, const char* color) {
	eraseGhostFromOldPosition(ghostNumber);  // Erase the ghost from its old position
	setGhostPosition(x, y, ghostNumber);  // Set the new position of the ghost
	lastSymbol[ghostNumber] = grid[y][x];  // Store the symbol of the new position
	grid[y][x] = symbol;  // Set the ghost symbol at the new position
	repaintGhost(ghostNumber, color);  // Repaint the ghost with its color
}

// Function to check if a given position collides with any ghost
bool Ghost::isCollidedWithGhost(int x, int y) {
	// Loop through all ghosts and check if any of them occupy the given position
	for (size_t i = 0; i < amountOfGhosts; i++) {
		if (x == ghostX[i] && y == ghostY[i]) {
			return true;  // Return true if collision occurs
		}
	}
	return false;  // Return false if no collision occurs
}

// Function to check if moving up is clear for a ghost (no wall or collision)
bool Ghost::isDirectionUpClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY - 1][ghostCurX] != wallSymbol  // No wall above
		&& ((ghostCurY - 1) != lastY || ghostCurX != lastX)  // Not the previous position
		&& !isCollidedWithGhost(ghostCurX, ghostCurY - 1));  // No ghost collision above
}

// Function to check if moving left is clear for a ghost (no wall or collision)
bool Ghost::isDirectionLeftClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY][ghostCurX - 1] != wallSymbol  // No wall to the left
		&& (ghostCurY != lastY || (ghostCurX - 1) != lastX)  // Not the previous position
		&& !isCollidedWithGhost(ghostCurX - 1, ghostCurY));  // No ghost collision to the left
}

// Function to check if moving right is clear for a ghost (no wall or collision)
bool Ghost::isDirectionRightClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY][ghostCurX + 1] != wallSymbol  // No wall to the right
		&& ((ghostCurY) != lastY || (ghostCurX + 1) != lastX)  // Not the previous position
		&& !isCollidedWithGhost(ghostCurX + 1, ghostCurY));  // No ghost collision to the right
}

// Function to check if moving down is clear for a ghost (no wall or collision)
bool Ghost::isDirectionDownClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY + 1][ghostCurX] != wallSymbol  // No wall below
		&& ((ghostCurY + 1) != lastY || (ghostCurX) != lastX)  // Not the previous position
		&& !isCollidedWithGhost(ghostCurX, ghostCurY + 1));  // No ghost collision below
}

// Function to check if moving backwards is possible for a ghost based on its current position
bool Ghost::isBackwardsPossible(int x, int y) {
	// make sure that the coords given are in bounds
	if (x > 0 && y > 0 && x < 50 && y < 50) {
		// Check if the grid position is either a point or empty space, allowing the ghost to move backwards
		return (grid[y][x] == pointSymbol || grid[y][x] == emptySymbol);
	}
	return false;
}

// Function to make all ghosts move backwards if their respective score conditions are met
void Ghost::makeAllBackwardsMove() {//used in frightened mode first move
	// If the player's score meets the thresholds, move the corresponding ghost backwards
	if (playerScore >= scoreToActivtRed) {
		if (isBackwardsPossible(redLastX, redLastY)) {
			redGhost->moveBackwards(blueColor);
		}
	}
	if (playerScore >= scoreToActivtPink) {
		if (isBackwardsPossible(pinkLastX, pinkLastY)) {
			pinkGhost->moveBackwards(blueColor);
		}
	}
	if (playerScore >= scoreToActivtBlue) {
		if (isBackwardsPossible(blueLastX, blueLastY)) {
			blueGhost->moveBackwards(blueColor);
		}
	}
	if (playerScore >= scoreToActivtGreen) {
		if (isBackwardsPossible(greenLastX, greenLastY)) {
			greenGhost->moveBackwards(blueColor);
		}
	}
}

// Function to move a specific ghost backwards based on its current number (e.g., Blinky, Pinky, etc.)
void Ghost::moveGhostBack(int curGhostNumber, const char* color) {
	if (curGhostNumber == blinkyNumber) {
		redGhost->moveBackwards(color);  // Move Blinky backwards
	}
	else if (curGhostNumber == pinkyNumber) {
		pinkGhost->moveBackwards(color);  // Move Pinky backwards
	}
	else if (curGhostNumber == inkyNumber) {
		blueGhost->moveBackwards(color);  // Move Inky backwards
	}
	else if (curGhostNumber == clydeNumber) {
		greenGhost->moveBackwards(color);  // Move Clyde backwards
	}
}

// Function to change the position of a ghost based on the shortest path, or if it can't move, move it backwards or stay
void Ghost::ghostChangePosition(int ghostCurX, int ghostCurY, int distance[4],
	int ghostNumber, int ghostSymbol, const char* color, int& lastX, int& lastY) {
	int nextX = ghostCurX;
	int nextY = ghostCurY;
	// If the ghost can't move, attempt to move it backwards
	if (cantMove) {
		if (isBackwardsPossible(lastX, lastY)) {
			moveGhostBack(ghostNumber, color);  // Move the ghost backwards
			cantMove = false;  // Reset the cantMove flag
			return;
		}
		else {
			//no valid move for the ghost 
			cantMove = false;  // Reset the cantMove flag
			return;
		}
	}

	int minDistance = INT_MAX;  // Store the minimum distance to Pac-Man
	int direction = -1;  // Store the direction to move in (Up, Left, Down, Right)

	// Evaluate all 4 possible directions (Up, Left, Down, Right) and find the shortest path
	for (int i = 0; i < 4; i++) {
		if (distance[i] < minDistance) {
			minDistance = distance[i];  // Update the minimum distance
			direction = i;  // Store the best direction to move in
		}
	}

	// Set the next position of the ghost based on the chosen direction
	switch (direction) {
	case 0: nextY = ghostCurY - 1; break; // Up
	case 1: nextX = ghostCurX - 1; break; // Left
	case 2: nextY = ghostCurY + 1; break; // Down
	case 3: nextX = ghostCurX + 1; break; // Right
	}

	// Move the ghost to the selected position and update its last position
	moveGhostTo(nextX, nextY, ghostNumber, ghostSymbol, color);
	lastX = ghostCurX;  // Update the last known X position
	lastY = ghostCurY;  // Update the last known Y position
}

// Function to check the availability of directions (Up, Left, Down, Right) for the ghost to move
void Ghost::checkDirectionsAvailability(int ghostCurX, int ghostCurY, int distance[], int changeX, int changeY, int lastX, int lastY) {
	int noDirectionClear = 4;  // Count the number of directions that are clear

	// Check if each direction (Up, Left, Down, Right) is clear
	if (isDirectionUpClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[0] = Pacman::distanceToPacman(ghostCurX, ghostCurY - 1, changeX, changeY);  // Calculate the distance for Up
		noDirectionClear--;  // Decrement the count of directions that are clear
	}
	if (isDirectionLeftClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[1] = Pacman::distanceToPacman(ghostCurX - 1, ghostCurY, changeX, changeY);  // Calculate the distance for Left
		noDirectionClear--;  // Decrement the count of directions that are clear
	}
	if (isDirectionDownClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[2] = Pacman::distanceToPacman(ghostCurX, ghostCurY + 1, changeX, changeY);  // Calculate the distance for Down
		noDirectionClear--;  // Decrement the count of directions that are clear
	}
	if (isDirectionRightClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[3] = Pacman::distanceToPacman(ghostCurX + 1, ghostCurY, changeX, changeY);  // Calculate the distance for Right
		noDirectionClear--;  // Decrement the count of directions that are clear
	}

	// If no direction is clear, set cantMove flag to true
	if (noDirectionClear == 0) {
		cantMove = true;
	}
}

// Function to check if a ghost has been eaten by Pac-Man by checking their positions
int Ghost::isGhostEatenByPacman() {
	// Pacman must be in frightened mode to eat a ghost
	if (!isFrightenedMode) {
		return -1;
	}

	// Check if Pacman is stationary (not moving)
	if (!isGoingUp && !isGoingDown && !isGoingLeft && !isGoingRight) {
		return -1;  // Pacman cannot eat a ghost while standing still
	}

	for (int i = 0; i < amountOfGhosts; i++) {
		// Check if Pacman's position matches a ghost's position
		if (getGhostXPosition(i) == pacmanX && getGhostYPosition(i) == pacmanY) {
			return i;  // Return the index of the eaten ghost
		}
	}
	return -1;  // No ghost was eaten
}

// Function to check if a corner is available for a ghost to move into
bool Ghost::isCornerAvailable(int x, int y) {
	return false;  // This function always returns false (not implemented yet)
}

// Function to check if a corner is clear for a ghost to move into
bool Ghost::isCornerClear(int x, int y) {
	// Check if the grid position is either a point or empty space, allowing the ghost to move into the corner
	return (grid[y][x] == pointSymbol) || (grid[y][x] == emptySymbol);
}

// Function to send a ghost to a corner on the grid
void Ghost::sendGhostToCorner(int ghostNumber) {
	int curHeightOfGrid = heightGrid - wallsIncl;
	int curWidthOfGrid = widthGrid - wallsIncl;

	switch (ghostNumber) {
	case 0:
		if (isCornerClear(1, curWidthOfGrid - 1)) {
			moveGhostTo(1, curWidthOfGrid - 1, blinkyNumber, blinkySymbol, redColor);  // Move Blinky to corner
		}
		else if (isCornerClear(2, curWidthOfGrid - 1)) {
			moveGhostTo(2, curWidthOfGrid - 1, blinkyNumber, blinkySymbol, redColor);  // Move Blinky to adjacent position
		}
		break;
	case 1:
		if (isCornerClear(1, 1)) {
			moveGhostTo(1, 1, pinkyNumber, pinkySymbol, pinkColor);  // Move Pinky to corner
		}
		else if (isCornerClear(1, 2)) {
			moveGhostTo(1, 2, pinkyNumber, pinkySymbol, pinkColor);  // Move Pinky to adjacent position
		}
		break;
	case 2:
		if (isCornerClear(curHeightOfGrid - 1, curWidthOfGrid - 1)) {
			moveGhostTo(curHeightOfGrid - 1, curWidthOfGrid - 1, inkyNumber, inkySymbol, blueColor);  // Move Inky to corner
		}
		else if (isCornerClear(curHeightOfGrid - 2, curWidthOfGrid - 1)) {
			moveGhostTo(curHeightOfGrid - 2, curWidthOfGrid - 1, inkyNumber, inkySymbol, blueColor);  // Move Inky to adjacent position
		}
		break;
	case 3:
		if (isCornerClear(curHeightOfGrid - 1, 1)) {
			moveGhostTo(curHeightOfGrid - 1, 1, clydeNumber, clydeSymbol, greenColor);  // Move Clyde to corner
		}
		else if (isCornerClear(curHeightOfGrid - 2, 1)) {
			moveGhostTo(curHeightOfGrid - 2, 1, clydeNumber, clydeSymbol, greenColor);  // Move Clyde to adjacent position
		}
		break;
	}
}

// Function to activate ghosts based on the player's score
int Ghost::activateReadyGhosts() {
	int curAmountOfGhosts = 0;  // Initialize the count of active ghosts to 0

	// Check player's score to determine which ghosts should be activated
	if (playerScore >= scoreToActivtRed) {
		curAmountOfGhosts = 1;  // Activate 1 ghost (Red) if score is high enough
	}
	if (playerScore >= scoreToActivtPink) {
		curAmountOfGhosts = 2;  // Activate 2 ghosts (Red + Pink) if score is high enough
	}
	if (playerScore >= scoreToActivtBlue) {
		curAmountOfGhosts = 3;  // Activate 3 ghosts (Red + Pink + Blue) if score is high enough
	}
	if (playerScore >= scoreToActivtGreen) {
		curAmountOfGhosts = 4;  // Activate 4 ghosts (Red + Pink + Blue + Green) if score is high enough
	}

	return curAmountOfGhosts;  // Return the total number of active ghosts
}

bool Ghost::tryBackwardsMovement(int validDirectionsCount, int lastX, int lastY, int ghostNumber, char ghostSymbol, const char* color) {
	if (validDirectionsCount == 0) {
		if (isBackwardsPossible(lastX, lastY)) {
			// Implement the backwards movement logic
			if (ghostNumber == blinkyNumber) {
				moveGhostTo(lastX, lastY, ghostNumber, ghostSymbol, color); // Move Blinky back
			}
			else if (ghostNumber == pinkyNumber) {
				moveGhostTo(lastX, lastY, ghostNumber, ghostSymbol, color); // Move Pinky back
			}
			else if (ghostNumber == inkyNumber) {
				moveGhostTo(lastX, lastY, ghostNumber, ghostSymbol, color); // Move Inky back
			}
			else if (ghostNumber == clydeNumber) {
				moveGhostTo(lastX, lastY, ghostNumber, ghostSymbol, color); // Move Clyde back
			}
		}
		return false; // Return to prevent random movement if no directions are valid
	}
	return true;
}

void Ghost::tryRandomValidDirect(bool directionValid[], int ghostCurX, int ghostCurY, int ghostNumber, char ghostSymbol, const char* color) {
	int randomWay = -1;
	bool isValid = false;
	int attempts = 0;
	while (!isValid && attempts < 4) {
		randomWay = rand() % 4;
		if (directionValid[randomWay]) {
			isValid = true; // Found a valid direction
		}
		attempts++;
	}

	// Move the ghost in the selected direction
	if (isValid) {
		switch (randomWay) {
		case 0: // Move Up
			moveGhostTo(ghostCurX, ghostCurY - 1, ghostNumber, ghostSymbol, color);
			isGhostEatenByPacman();
			break;
		case 1: // Move Down
			moveGhostTo(ghostCurX, ghostCurY + 1, ghostNumber, ghostSymbol, color);
			isGhostEatenByPacman();
			break;
		case 2: // Move Left
			moveGhostTo(ghostCurX - 1, ghostCurY, ghostNumber, ghostSymbol, color);
			isGhostEatenByPacman();
			break;
		case 3: // Move Right
			moveGhostTo(ghostCurX + 1, ghostCurY, ghostNumber, ghostSymbol, color);
			isGhostEatenByPacman();
			break;
		}
	}
}

// Function to move ghosts randomly within the grid based on available directions
void Ghost::randomGhostMoves(int ghostCurX, int ghostCurY, int& lastX, int& lastY, int ghostNumber, char ghostSymbol, const char* color) {
	int eatenGhost = isGhostEatenByPacman();  // Check if any ghost has been eaten by Pacman
	if (eatenGhost != -1) {
		sendGhostToCorner(eatenGhost);  // If a ghost was eaten, send it to its starting corner
		return;  // Exit the function after a ghost is eaten to prevent further movement
	}
	// Check the availability of directions
	bool directionValid[4] = {
			isDirectionUpClear(ghostCurX, ghostCurY, lastX, lastY),
			isDirectionDownClear(ghostCurX, ghostCurY, lastX, lastY),
			isDirectionLeftClear(ghostCurX, ghostCurY, lastX, lastY),
			isDirectionRightClear(ghostCurX, ghostCurY, lastX, lastY)
	};

	// Count valid directions
	int validDirectionsCount = 0;
	for (int i = 0; i < 4; i++) {
		if (directionValid[i]) {
			validDirectionsCount++;
		}
	}

	// If no valid direction is available, move backwards
	if (!tryBackwardsMovement(validDirectionsCount, lastX, lastY, ghostNumber, ghostSymbol, color)) {
		return;
	}

	// Otherwise, move in a random valid direction
	tryRandomValidDirect(directionValid, ghostCurX, ghostCurY, ghostNumber, ghostSymbol, color);

	// Update the last position of the ghost after moving
	lastX = ghostCurX;
	lastY = ghostCurY;
	lastY = ghostCurY;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//function that activates the frigthened mode
void Ghost::frightenedModeActivated(int ghostCurX, int ghostCurY, int& lastX, int& lastY, int ghostNumber, char ghostSymbol, const char* color) {
	randomGhostMoves(ghostCurX, ghostCurY, lastX, lastY, ghostNumber, ghostSymbol, color);
	repaintGhost(ghostNumber, blueColor);
}

//iterates through the ghosts that are awake and turns on the their frightened Mode
void Ghost::frightenedModeGhosts() {
	int curAmountOfGhosts = activateReadyGhosts();
	for (int i = 0; i < curAmountOfGhosts; i++) {
		int curGhostX = getGhostXPosition(i);
		int curGhostY = getGhostYPosition(i);
		switch (i) {
		case 0:
			frightenedModeActivated(curGhostX, curGhostY, redLastX, redLastY, i, blinkySymbol, redColor);//blinky frightened Mode 
			break;
		case 1:
			frightenedModeActivated(curGhostX, curGhostY, pinkLastX, pinkLastY, i, pinkySymbol, pinkColor);//pinky frightened Mode 
			break;
		case 2:
			frightenedModeActivated(curGhostX, curGhostY, blueLastX, blueLastY, i, inkySymbol, blueColor);//inky frightened Mode 
			break;
		case 3:
			frightenedModeActivated(curGhostX, curGhostY, greenLastX, greenLastY, i, clydeSymbol, greenColor);//clyde frightened Mode 
			break;
		}
	}
}

// Activate ghosts based on the player's score
void Ghost::activateGhosts() {
	// Check the player's score and activate ghosts accordingly
	if (playerScore >= redGhost->getScoreToActivate()) {
		redGhost->activateGhost();  // Activate Red Ghost
	}
	if (playerScore >= pinkGhost->getScoreToActivate()) {
		pinkGhost->activateGhost(); // Activate Pink Ghost
	}
	if (playerScore >= blueGhost->getScoreToActivate()) {
		blueGhost->activateGhost();  // Activate Blue Ghost
	}
	if (playerScore >= greenGhost->getScoreToActivate()) {
		greenGhost->activateGhost();  // Activate Green Ghost
	}
	// Reset Pacman's movement direction after activating ghosts
	Pacman::pacmanMovmentDirectionReset();
}
