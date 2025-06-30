//Solution to course project # <5>
//Introduction to programming course
//Faculty of Mathematics and Informatics od Sofia University
//Winter semester 2024/2025
//
//@author <Boris Dimitrov Tsvetkov>
//idNumber <6MI0600504
//@compiler Microsoft Visual Studio compiler
//file with the main game and helper functions

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h> 
#include <fstream>
#include "Settings.h"
#include "Console.h"
#include "Game.h"
#include "Pacman.h"
#include "Ghost.h"

bool Game::isAllFoodCollected() {
	for (size_t i = 0; i < foodAmount; i++) {
		if (grid[foodY[i]][foodX[i]] == foodSymbol) {
			return false;
		}
	}
	return true;
}

// Function to check if the player has collected all the food (4 pieces)
bool Game::isPlayerCollectedAllFood() {
	return (collectedAmountOfFood == 4);  // Return true if all food is collected
}

// Function to check if the game is over
bool Game::isGameOver() {
	return Pacman::isPacmanCaughtByGhost() || (isAllFoodCollected() && isPlayerCollectedAllFood());
}

// Function to show the game over screen with a message and decide win or loss
void Game::showGameOverScreen() {
	Console::resizeConsole(20, 32);  // Resize console for the game over screen
	Console::clearConsole();  // Clear the console screen

	// Display the "Game Over" message in ASCII art
	std::cout << R"(
  ####       #      #      # #######      #####  #         # ####### #####    #
 #    #     # #     ##    ## #           #     #  #       #  #       #    #   #
 #         #   #    # #  # # #           #     #   #     #   #       #####    #
 #  ###   #######   #  ##  # #######     #     #    #   #    ####### #   #    #
 #    #  #       #  #      # #           #     #     # #     #       #    #   
  ####  #         # #      # #######      #####       #      ####### #    #   #
    )" << std::endl;

	Sleep(1500);  

	// Determine if the game was won or lost based on collected food or being caught by a ghost
	if ((isAllFoodCollected() && isPlayerCollectedAllFood())) {
		Console::showWinScreen();  // Show the win screen
	}
	else if (Pacman::isPacmanCaughtByGhost()) {
		Console::showLoseScreen();  // Show the lose screen if Pacman is caught by a ghost
	}
}

// Function to spawn food on the grid in random empty positions
void Game::spawnFood() {
	for (int i = 0; i < foodAmount; i++) {
		int foodYCur = 0, foodXCur = 0;
		do {
			foodYCur = rand() % (heightGrid - 2) + 1;  // Random Y position within bounds
			foodXCur = rand() % (widthGrid - 2) + 1;   // Random X position within bounds
		} while (grid[foodYCur][foodXCur] != pointSymbol);  // Ensure food spawns on an empty space
		foodX[i] = foodXCur;
		foodY[i] = foodYCur;
		grid[foodYCur][foodXCur] = foodSymbol;  // Set food on the grid
		Console::setConsoleCursorPosition(foodXCur * 2, foodYCur);  // Position the cursor to display food
		std::cout << yellowColor << foodSymbol << endColor;  // Display the food
	}
}

// Function to check if the given symbol represents food
bool Game::isFoodEaten(char ch) {
	return ch == foodSymbol;  // Return true if the symbol is food
}

// Function to check if the given symbol represents a point
bool Game::isPointEaten(char ch) {
	return ch == pointSymbol;  // Return true if the symbol is a point
}

// Function to update a specific grid cell with a new symbol
void Game::updateGridCell(int x, int y, char symbol) {
	Console::setConsoleCursorPosition(x * 2, y);  // Position the cursor
	std::cout << yellowColor << symbol << endColor;  // Display the symbol in the grid cell
}

// Call chase mode, activating the ghosts and handling Pacman's movement
void Game::callChaseMode() {
	Pacman::handlePacmanMovement();  // Handle Pacman's movement
	Ghost::activateGhosts();  // Activate ghosts based on the current score
	Console::displayPlayerScore();  // Display the current player score
}

// Call frightened mode, which either makes ghosts move backward or perform normal frightened behavior
void Game::callFrightenedMode() {
	Pacman::handlePacmanMovement();  // Handle Pacman's movement during frightened mode
	if (frightenedModeCount == 0) {
		Ghost::makeAllBackwardsMove();  // Make ghosts move backward if the frightened mode count is 0
	}
	else {
		Ghost::frightenedModeGhosts();  // Otherwise, apply frightened mode behavior for ghosts
	}
	Console::displayPlayerScore();  // Display the current player score
}

// End the frightened mode, resetting the state to chase mode
void Game::endFrightenedMode() {
	isFrightenedMode = false;  // Disable frightened mode
	isChaseMode = true;  // Enable chase mode
	frightenedModeCount = 0;  // Reset the frightened mode count
}

// Main game loop that keeps running until the game is over
void Game::runGameLoop() {
	// Game continues as long as the game is not over
	while (!Game::isGameOver()) {
		// If in chase mode, call the chase mode function
		if (isChaseMode) {
			callChaseMode();
			Sleep(125);  // Pause for a short period to control the game speed
		}
		// If in frightened mode, call the frightened mode function
		if (isFrightenedMode) {
			callFrightenedMode();
			Sleep(125);  // Pause for a short period to control the game speed
			frightenedModeCount++;  // Increment the frightened mode count
			// End the frightened mode after 20 cycles (adjust this number to your game rules)
			if (frightenedModeCount == 30) {
				endFrightenedMode();
			}
		}
	}
	Sleep(600);  // After game ends, sleep for a short time before displaying the game over screen
	Game::showGameOverScreen();  // Show the game over screen
}

// Initialize the game environment (grid, map, ghosts, etc.)
void Game::InitializeGame() {
	Console::hideConsoleCursor();  // Hide the console cursor to avoid cluttering the screen
	srand((unsigned int)time(0));  // Seed the random number generator for randomness
	Console::initializeGridDimensions();  // Set up grid dimensions for the game
	Console::loadMapFromFile();  // Load the map from a file (where the game layout is stored)
	Console::renderGrid();  // Render the grid (game area) on the screen
	Ghost::spawnGhost();  // Spawn the ghosts at their initial positions
	Pacman::spawnPacman();  // Spawn Pacman at the starting position
	Game::spawnFood();  // Spawn food on the map
}

// Start the Pacman game, initializing it and then running the game loop
void Game::startPacmanGame() {
	InitializeGame();  // Initialize the game environment
	Sleep(3000);  // Wait for 3 seconds before starting the game (to give the player time to prepare)
	runGameLoop();  // Start the main game loop
}

