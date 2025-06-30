#include "Pacman.h"
#include "Settings.h"
#include <stdlib.h>
#include "Console.h"
#include <iostream>
#include "Game.h"
#include <Windows.h>


// Function to check if Pacman has been caught by a ghost during chase mode
bool Pacman::isPacmanCaughtByGhost() {
	if (isChaseMode) {
		for (size_t i = 0; i < amountOfGhosts; i++) {
			// Check if Pacman and ghost occupy the same position
			if (ghostX[i] == pacmanX && ghostY[i] == pacmanY) {
				return true;
			}

			// Get the last position of the ghost
			int lastX, lastY;
			switch (i) {
			case 0: lastX = redLastX; lastY = redLastY; break;
			case 1: lastX = pinkLastX; lastY = pinkLastY; break;
			case 2: lastX = blueLastX; lastY = blueLastY; break;
			case 3: lastX = greenLastX; lastY = greenLastY; break;
			}

			// Convert movement direction booleans into integer values
			int moveX = (isGoingRight ? 1 : 0) - (isGoingLeft ? 1 : 0);
			int moveY = (isGoingDown ? 1 : 0) - (isGoingUp ? 1 : 0);

			// Check if Pacman and ghost swapped positions
			if (ghostX[i] == pacmanX - moveX && ghostY[i] == pacmanY - moveY &&
				pacmanX == lastX && pacmanY == lastY) {
				return true;
			}
		}
	}
	return false;
}

// Function to spawn Pacman at a random empty position on the grid
void Pacman::spawnPacman() {
	do {
		pacmanX = rand() % (widthGrid - 2) + 1;  // Random X position within bounds
		pacmanY = rand() % (heightGrid - 2) + 1; // Random Y position within bounds
	} while (grid[pacmanY][pacmanX] != pointSymbol);  // Ensure Pacman spawns on an empty space
	grid[pacmanY][pacmanX] = pacmanSymbol;  // Set Pacman on the grid
	Console::setConsoleCursorPosition(pacmanX * 2, pacmanY);  // Position the cursor to display Pacman
	std::cout << yellowColor << pacmanSymbol << endColor;  // Display Pacman
}

// Function to erase Pacman from its old position
void Pacman::erasePacmanFromOldPosition() {
	Game::updateGridCell(pacmanX, pacmanY, emptySymbol);  // Clear the old position
}

// Function to update Pacman's position on the grid
void Pacman::updatePacmanGridPosition() {
	Game::updateGridCell(pacmanX, pacmanY, pacmanSymbol);  // Update the grid cell with Pacman's symbol
}

// Function to clear Pacman's previous cell in the grid
void Pacman::clearPacmanPreviousCell() {
	grid[pacmanY][pacmanX] = emptySymbol;  // Set the previous cell to empty
}

// Function to set Pacman's current position in the grid
void Pacman::setPacmanPosition() {
	grid[pacmanY][pacmanX] = pacmanSymbol;  // Place Pacman on the grid at the current position
}

// Function to move Pacman in a specified direction
void Pacman::movePacmanInDirection(int movingX, int movingY, char nextSymbol) {
	// Handle food collection and change state if Pacman eats food or point
	if (Game::isFoodEaten(nextSymbol)) {
		Console::updateScore(prizeOfFood);  // Update the score for food
		collectedAmountOfFood++;  // Increment the food collected counter
		isFrightenedMode = true;  // Activate frightened mode
		isChaseMode = false;  // Deactivate chase mode
		frightenedModeCount = 0; //reset count
	}
	else if (Game::isPointEaten(nextSymbol)) {
		Console::updateScore(prizeOfPoint);  // Update the score for the point
	}
	clearPacmanPreviousCell();  // Clear the previous position of Pacman
	erasePacmanFromOldPosition();  // Erase Pacman from the old position
	pacmanY += movingY;  // Update Pacman's Y position based on direction
	pacmanX += movingX;  // Update Pacman's X position based on direction
	setPacmanPosition();  // Set Pacman's new position on the grid
	updatePacmanGridPosition();  // Update the grid with the new position
}

// Function to move Pacman up if there is no wall
void Pacman::movePacmanUp() {
	if (grid[pacmanY - 1][pacmanX] != wallSymbol) {  // Check if the next cell is not a wall
		char nextSymbol = grid[pacmanY - 1][pacmanX];  // Get the symbol of the next cell
		movePacmanInDirection(0, -1, nextSymbol);  // Move Pacman up
	}
}

// Function to move Pacman down if there is no wall
void Pacman::movePacmanDown() {
	if (grid[pacmanY + 1][pacmanX] != wallSymbol) {  // Check if the next cell is not a wall
		char nextSymbol = grid[pacmanY + 1][pacmanX];  // Get the symbol of the next cell
		movePacmanInDirection(0, 1, nextSymbol);  // Move Pacman down
	}
}

// Function to move Pacman left if there is no wall
void Pacman::movePacmanLeft() {
	if (grid[pacmanY][pacmanX - 1] != wallSymbol) {  // Check if the next cell is not a wall
		char nextSymbol = grid[pacmanY][pacmanX - 1];  // Get the symbol of the next cell
		movePacmanInDirection(-1, 0, nextSymbol);  // Move Pacman left
	}
}

// Function to move Pacman right if there is no wall
void Pacman::movePacmanRight() {
	if (grid[pacmanY][pacmanX + 1] != wallSymbol) {  // Check if the next cell is not a wall
		char nextSymbol = grid[pacmanY][pacmanX + 1];  // Get the symbol of the next cell
		movePacmanInDirection(1, 0, nextSymbol);  // Move Pacman right
	}
}

// Function to handle Pacman's movement based on key press (WASD keys)
void Pacman::handlePacmanMovement() {
	const int KEY_PRESSED_MASK = 0x8000;  // Mask to check if a key is pressed
	if (GetAsyncKeyState('W') & KEY_PRESSED_MASK) {  // W key pressed (up)
		isGoingUp = true;
		movePacmanUp();  // Move Pacman up if 'W' key is pressed
		isPacmanCaughtByGhost();
	}
	if (GetAsyncKeyState('S') & KEY_PRESSED_MASK) {  // S key pressed (down)
		isGoingDown = true;
		movePacmanDown();  // Move Pacman down if 'S' key is pressed
		isPacmanCaughtByGhost();
	}
	if (GetAsyncKeyState('A') & KEY_PRESSED_MASK) {  // A key pressed (left)
		isGoingLeft = true;
		movePacmanLeft();  // Move Pacman left if 'A' key is pressed
		isPacmanCaughtByGhost();
	}
	if (GetAsyncKeyState('D') & KEY_PRESSED_MASK) {  // D key pressed (right)
		isGoingRight = true;
		movePacmanRight();  // Move Pacman right if 'D' key is pressed
		isPacmanCaughtByGhost();
	}
}

// Function to calculate the Manhattan distance between the ghost and the predicted position of Pac-Man
int Pacman::distanceToPacman(int ghostCurX, int ghostCurY, int changeX, int changeY) {
	// Manhattan distance formula: sum of the absolute differences of the X and Y coordinates
	return abs(ghostCurX - (pacmanX + changeX)) + abs(ghostCurY - (pacmanY + changeY));
}


// Reset the movement direction of Pacman (used after each mode change)
void Pacman::pacmanMovmentDirectionReset() {
	isGoingDown = false;
	isGoingUp = false;
	isGoingRight = false;
	isGoingLeft = false;
}
