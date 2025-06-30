#pragma once
class Console
{
public:
	// Function to set the console cursor position (move the cursor to a specific (x, y) coordinate)
	static void setConsoleCursorPosition(int x, int y);
	// Function to hide the console cursor (makes it invisible)
	static void hideConsoleCursor();
	// Function to clear the console screen (removes all output on the console)
	static void clearConsole();
	// Function to resize the console window (set its height and width)
	static void resizeConsole(int height, int width);
	// Function to center the console window on the screen
	static void centerConsole();
	//	
	static void initializeGridMemory(int rows, int cols);
	// Function to free dynamically allocated memory for the grid
	static void freeGridMemory();
	// Function to skip a line in the file
	static void skipLineInFile();
	// Function to add boundary walls around the grid
	static void addBoundaryWalls();
	// Function to load a map from a file into the grid
	static void loadMapFromFile();
	// Function to render and display the grid in the console
	static void renderGrid();
	// Function to prepare the console window for the game
	static void prepareConsoleForGame();
	// Function to initialize grid dimensions based on user input
	static void initializeGridDimensions();
	// Function to wait for any key press to exit the game
	static void waitForGameExit();
	// Function to display the player's current score on the console
	static void displayPlayerScore();
	// Function to update the player's score by a specified number of points
	static void updateScore(int points);
	//screen is shown when the game is lost
	static void showLoseScreen();
	//this screen is shown when the player wins and collect all the fruit
	static void showWinScreen();
};

