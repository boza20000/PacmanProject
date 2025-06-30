#pragma once

class Game {
	Game(){}
public:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	static Game& game() {
		static Game instance;
		return instance;
	}

	static bool isAllFoodCollected();
	// Function to check if the player has collected all the food (4 pieces)
	static bool isPlayerCollectedAllFood();

	// Function to check if the game is over
	static bool isGameOver();
	// Function to show the game over screen with a message and decide win or loss
	static void showGameOverScreen();

	// Function to spawn food on the grid in random empty positions
	static void spawnFood();

	// Function to check if the given symbol represents food
	static bool isFoodEaten(char ch);

	// Function to check if the given symbol represents a point
	static bool isPointEaten(char ch);

	// Function to update a specific grid cell with a new symbol
	static void updateGridCell(int x, int y, char symbol);

	// Call chase mode, activating the ghosts and handling Pacman's movement
	void callChaseMode();

	// Call frightened mode, which either makes ghosts move backward or perform normal frightened behavior
	void callFrightenedMode();

	// End the frightened mode, resetting the state to chase mode
	void endFrightenedMode();

	// Main game loop that keeps running until the game is over
	void runGameLoop();

	// Initialize the game environment (grid, map, ghosts, etc.)
	void InitializeGame();

	// Start the Pacman game, initializing it and then running the game loop
	void startPacmanGame();
};
