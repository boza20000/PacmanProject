#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h> 
#include <fstream>
#include "Pacman.h"
#include "map.h"
#include "RedGhost.h"
#include "BlueGhost.h"
#include "GreenGhost.h"
#include "PinkGhost.h"

const size_t amountOfGhosts = 4;
const size_t foodAmount = 4;
const size_t prizeOfFood = 20;
const size_t prizeOfPoint = 1;
int ghostX[amountOfGhosts], ghostY[amountOfGhosts];
int foodX[foodAmount], foodY[foodAmount];
char lastSymbol[amountOfGhosts] = { ' ',' ',' ',' ' };
const size_t wallsIncl = 2;
const char* blueColor = "\033[34m";
const char* endColor = "\033[0m";
size_t collectedAmountOfFood = 0;
size_t frightenedModeCount = 0;
bool isChaseMode = true;
bool isFrightenedMode = false;
bool cantMove = false;
std::ifstream file("map.txt");

// Function to set the cursor position at a specific (x, y) coordinate in the console window
void setConsoleCursorPosition(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to the console output
	COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };  // Create a COORD structure to store the position
	SetConsoleCursorPosition(hConsole, position);  // Set the cursor position to the specified (x, y)
}

// Function to hide the console cursor (makes it invisible)
void hideConsoleCursor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to the console output
	CONSOLE_CURSOR_INFO cursorInfo;  // Declare a CONSOLE_CURSOR_INFO structure to hold cursor visibility information
	cursorInfo.dwSize = 1;  // Set the cursor size (a smaller value makes the cursor "invisible")
	cursorInfo.bVisible = FALSE;  // Set the cursor visibility to false (hide it)
	SetConsoleCursorInfo(hConsole, &cursorInfo);  // Apply the cursor settings
}

// Function to clear the console screen (removes all output on the console)
void clearConsole() {
	system("cls");  // Execute the system command "cls" to clear the console
}

// Function to resize the console window (set its height and width)
void resizeConsole(int height, int width) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to the console output
	// Set the window size using a SMALL_RECT structure, which defines the window's top-left and bottom-right corners
	SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(width * 2 + 15), static_cast<SHORT>(height + 1) };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);  // Apply the new window size
}

// Function to center the console window on the screen
void centerConsole() {
	RECT screenRect;
	GetWindowRect(GetDesktopWindow(), &screenRect);  // Get the dimensions of the desktop screen
	int screenWidth = screenRect.right - screenRect.left;  // Calculate screen width
	int screenHeight = screenRect.bottom - screenRect.top;  // Calculate screen height
	HWND consoleWindow = GetConsoleWindow();  // Get handle to the console window
	RECT consoleRect;
	GetWindowRect(consoleWindow, &consoleRect);  // Get the dimensions of the console window
	int consoleWidth = consoleRect.right - consoleRect.left;  // Calculate console window width
	int consoleHeight = consoleRect.bottom - consoleRect.top;  // Calculate console window height
	// Calculate the position to center the console window on the screen
	int xPos = (screenWidth - consoleWidth) / 2 - 10;
	int yPos = (screenHeight - consoleHeight) / 2;
	// Move the console window to the calculated position
	MoveWindow(consoleWindow, xPos, yPos, consoleWidth, consoleHeight, TRUE);
}

// Function to dynamically allocate memory for a 2D grid and initialize it with spaces
void initializeGridMemory(int rows, int cols) {
	grid = new char* [rows];  // Allocate memory for rows
	for (int i = 0; i < rows; i++) {
		grid[i] = new char[cols];  // Allocate memory for each row
		for (int j = 0; j < cols; j++) {
			grid[i][j] = ' ';  // Initialize all cells with empty spaces
		}
	}
}

// Function to free dynamically allocated memory for the grid
void freeGridMemory() {
	if (grid != nullptr) {
		for (int i = 0; i < heightGrid; i++) {
			delete[] grid[i];  // Free memory for each row
		}
		delete[] grid;  // Free memory for the grid itself
		grid = nullptr;  // Set the grid pointer to null to avoid dangling reference
	}
}

// Function to skip a line in the file
void skipLineInFile() {
	file.ignore(std::streamsize(INT_MAX), '\n');  // Skip characters until newline
}

// Function to add boundary walls around the grid
void addBoundaryWalls() {
	for (size_t i = 0; i < heightGrid; i++) {
		for (size_t j = 0; j < widthGrid; j++) {
			if ((heightGrid - 1) == i || (widthGrid - 1) == j) {
				grid[i][j] = '#';  // Set boundary walls
			}
		}
	}
}

// Function to load a map from a file into the grid
void loadMapFromFile() {
	if (!file) {
		std::cerr << "Error: Could not open file" << std::endl;  // Check if file is open
		return;
	}
	for (int row = 0; row < heightGrid; row++) {
		for (int col = 0; col < widthGrid; col++) {
			char ch;
			if (file.get(ch)) {
				grid[row][col] = ch;  // Read character from file into grid
			}
		}
		skipLineInFile();  // Skip the newline character after each row
	}
	file.close();
	addBoundaryWalls();  // Add boundary walls after loading the map
}

// Function to render and display the grid in the console
void renderGrid() {
	for (int i = 0; i < heightGrid; ++i) {
		for (int j = 0; j < widthGrid; ++j) {
			std::cout << grid[i][j] << " ";  // Print each cell of the grid
		}
		std::cout << std::endl;  // Move to the next line after a row
	}
}

// Function to prepare the console window for the game
void prepareConsoleForGame() {
	if (heightGrid < 20 || widthGrid < 20) {
		resizeConsole(heightGrid + 20, widthGrid + 40);  // Resize console if too small
	}
	else {
		resizeConsole(heightGrid, widthGrid);  // Set console size to grid size
	}
	centerConsole();  // Center the console on the screen
	initializeGridMemory(heightGrid, widthGrid);  // Initialize grid memory
	clearConsole();  // Clear the console screen
}

// Function to check if a string represents a valid number
bool isNumber(char* str) {
	while (*str) {
		if ((*str) < '0' || (*str) > '9') {
			return false;  // Return false if any character is not a digit
		}
		str++;
	}
	return true;
}

// Function to convert a string to an integer
int toInt(char* str) {
	int number = 0;
	while (*str) {
		number = number * 10 + (*str - '0');  // Build the integer from digits
		str++;
	}
	return number;
}

// Function to initialize grid dimensions based on user input
void initializeGridDimensions() {
	centerConsole();  // Center the console window
	const int ARRAY_SIZE = 100;  // Maximum size for input arrays
	char number1[ARRAY_SIZE], number2[ARRAY_SIZE];  // Arrays to hold input for width and height

	while (true) {
		std::cout << "Input number for the field of the game" << std::endl;
		std::cout << "Enter grid width: ";
		std::cin.getline(number1, ARRAY_SIZE);  // Get input for width

		std::cout << "Enter grid height: ";
		std::cin.getline(number2, ARRAY_SIZE);  // Get input for height

		clearConsole();  // Clear the console screen
		// Check if inputs are valid numbers
		if (!isNumber(number1) || !isNumber(number2)) {
			std::cerr << "ERROR: Incorrect input, please enter numbers." << std::endl << std::endl;
			continue;
		}

		// Convert the string inputs to integers
		widthGrid = toInt(number1);
		heightGrid = toInt(number2);

		// Check if the dimensions are within valid range
		int diff = std::abs(widthGrid - heightGrid);
		if (widthGrid < 10 || heightGrid < 10 || widthGrid > 50 || heightGrid > 50) {
			std::cerr << "ERROR: Grid dimensions must be positive integers over 10 and under 50 (inclusive)" << std::endl << std::endl;
			continue;
		}
		// Ensure that the difference between width and height is not too large
		if (diff > 10) {
			std::cerr << "(height - width) should not be more than 10." << std::endl;
			continue;
		}

		// Include walls in the grid if dimensions are valid
		if (widthGrid <= 49) {
			widthGrid += wallsIncl;
		}

		if (heightGrid <= 49) {
			heightGrid += wallsIncl;
		}

		break;  // Exit the loop if valid dimensions are entered
	}
	prepareConsoleForGame();  // Prepare the console for the game with the initialized dimensions
}

// Function to wait for any key press to exit the game
void waitForGameExit() {
	// Infinite loop to check for key press
	while (true) {
		// Check standard ASCII range and function keys
		for (int key = 0x20; key <= 0x7E; ++key) {  // Printable ASCII characters
			if (GetAsyncKeyState(key) & 0x8000) {  // If key is pressed
				clearConsole();  // Clear the console
				std::cout << "Exiting game..." << std::endl;  // Provide feedback
				Sleep(1500);
				exit(0);
				return;  // Exit function safely
			}
		}
		// Sleep for 50ms to reduce CPU usage
		Sleep(50);
	}
}

// Function to display the player's current score on the console
void displayPlayerScore() {
	setConsoleCursorPosition(widthGrid * 2 + 1, heightGrid / 2);  // Set cursor position for score display
	std::cout << "Score: " << playerScore << "  ";  // Display the player's score
}

// Function to update the player's score by a specified number of points
void updateScore(int points) {
	playerScore += points;  // Update the score
	displayPlayerScore();  // Display the updated score
}


//screen is shown when the game is lost
void showLoseScreen() {
	resizeConsole(20, 28);
	clearConsole();
	centerConsole();
	std::cout << R"(
 #     #  #####  #     #     #        ######    #######  #######    #
  #   #  #     # #     #     #       #      #   #        #          #
   # #   #     # #     #     #       #      #   ###      #          #
    #    #     # #     #     #       #      #      ####  #######    #
    #    #     # #     #     #       #      #         #  #     
    #     #####   #####      #######  ######    #######  #######    # 
    )" << std::endl;

	std::cout << "\n\n          Your Final Score: " << playerScore << "\n\n";
	std::cout << "   Press any key to exit the game...";
	waitForGameExit();
}

//this screen is shown when the player wins and collect all the fruit
void showWinScreen() {
	resizeConsole(20, 25);
	clearConsole();
	centerConsole();
	std::cout << R"(
 #     #  #####  #     #     #      #  #######  ##    #    #
  #   #  #     # #     #     #      #     #     # #   #    #
   # #   #     # #     #     #  ##  #     #     #  #  #    # 
    #    #     # #     #     # #  # #     #     #   # #    #
    #    #     # #     #     ##    ##     #     #    ## 
    #     #####   #####      #      #  #######  #     #    #
    )" << std::endl;

	std::cout << "\n\n          Your Final Score: " << playerScore << "\n\n";

	std::cout << "   Press any key to exit the game...";
	waitForGameExit();
}

bool isAllFoodCollected() {
	for (size_t i = 0; i < foodAmount; i++) {
		if (grid[foodY[i]][foodX[i]] == foodSymbol) {
			return false;
		}
	}
	return true;
}

// Function to check if the player has collected all the food (4 pieces)
bool isPlayerCollectedAllFood() {
	return (collectedAmountOfFood == 4);  // Return true if all food is collected
}

// Function to check if Pacman has been caught by a ghost during chase mode
bool isPacmanCaughtByGhost() {
	if (isChaseMode) {
		for (size_t i = 0; i < amountOfGhosts; i++) {
			// Check if Pacman's current position matches a ghost's current position
			if (ghostX[i] == pacmanX && ghostY[i] == pacmanY) {
				return true;
			}

			// Variables to track the last position of each ghost
			int lastX, lastY;
			switch (i) {
			case 0: lastX = redLastX; lastY = redLastY; break;  // Red ghost (Blinky)
			case 1: lastX = pinkLastX; lastY = pinkLastY; break;  // Pink ghost (Pinky)
			case 2: lastX = blueLastX; lastY = blueLastY; break;  // Blue ghost (Inky)
			case 3: lastX = greenLastX; lastY = greenLastY; break;  // Green ghost (Clyde)
			}

			// Check if Pacman and the ghost swapped positions
			if (ghostX[i] == pacmanX - isGoingRight + isGoingLeft && ghostY[i] == pacmanY - isGoingDown + isGoingUp &&
				pacmanX == ghostX[i] - (ghostX[i] - lastX) && pacmanY == ghostY[i] - (ghostY[i] - lastY)) {
				return true;
			}
		}
	}
	return false;
}
// Function to check if the game is over
bool isGameOver() {
	return isPacmanCaughtByGhost() || (isAllFoodCollected() && isPlayerCollectedAllFood());
}

// Function to show the game over screen with a message and decide win or loss
void showGameOverScreen() {
	resizeConsole(20, 32);  // Resize console for the game over screen
	clearConsole();  // Clear the console screen

	// Display the "Game Over" message in ASCII art
	std::cout << R"(
  ####       #      #      # #######      #####  #         # ####### #####    #
 #    #     # #     ##    ## #           #     #  #       #  #       #    #   #
 #         #   #    # #  # # #           #     #   #     #   #       #####    #
 #  ###   #######   #  ##  # #######     #     #    #   #    ####### #   #    #
 #    #  #       #  #      # #           #     #     # #     #       #    #   
  ####  #         # #      # #######      #####       #      ####### #    #   #
    )" << std::endl;

	Sleep(1500);  // Pause for 1.5 seconds before proceeding

	// Determine if the game was won or lost based on collected food or being caught by a ghost
	if ((isAllFoodCollected() && isPlayerCollectedAllFood())) {
		showWinScreen();  // Show the win screen
	}
	else if (isPacmanCaughtByGhost()) {
		showLoseScreen();  // Show the lose screen if Pacman is caught by a ghost
	}
}

// Function to spawn food on the grid in random empty positions
void spawnFood() {
	for (int i = 0; i < foodAmount; i++) {
		int foodYCur = 0, foodXCur = 0;
		do {
			foodYCur = rand() % (heightGrid - 2) + 1;  // Random Y position within bounds
			foodXCur = rand() % (widthGrid - 2) + 1;   // Random X position within bounds
		} while (grid[foodYCur][foodXCur] != pointSymbol);  // Ensure food spawns on an empty space
		foodX[i] = foodXCur;
		foodY[i] = foodYCur;
		grid[foodYCur][foodXCur] = foodSymbol;  // Set food on the grid
		setConsoleCursorPosition(foodXCur * 2, foodYCur);  // Position the cursor to display food
		std::cout << yellowColor << foodSymbol << endColor;  // Display the food
	}
}

// Function to check if the given symbol represents food
bool isFoodEaten(char ch) {
	return ch == foodSymbol;  // Return true if the symbol is food
}

// Function to check if the given symbol represents a point
bool isPointEaten(char ch) {
	return ch == pointSymbol;  // Return true if the symbol is a point
}

// Function to spawn Pacman at a random empty position on the grid
void spawnPacman() {
	do {
		pacmanX = rand() % (widthGrid - 2) + 1;  // Random X position within bounds
		pacmanY = rand() % (heightGrid - 2) + 1; // Random Y position within bounds
	} while (grid[pacmanY][pacmanX] != pointSymbol);  // Ensure Pacman spawns on an empty space
	grid[pacmanY][pacmanX] = pacmanSymbol;  // Set Pacman on the grid
	setConsoleCursorPosition(pacmanX * 2, pacmanY);  // Position the cursor to display Pacman
	std::cout << yellowColor << pacmanSymbol << endColor;  // Display Pacman
}

// Function to update a specific grid cell with a new symbol
void updateGridCell(int x, int y, char symbol) {
	setConsoleCursorPosition(x * 2, y);  // Position the cursor
	std::cout << yellowColor << symbol << endColor;  // Display the symbol in the grid cell
}

// Function to erase Pacman from its old position
void erasePacmanFromOldPosition() {
	updateGridCell(pacmanX, pacmanY, emptySymbol);  // Clear the old position
}

// Function to update Pacman's position on the grid
void updatePacmanGridPosition() {
	updateGridCell(pacmanX, pacmanY, pacmanSymbol);  // Update the grid cell with Pacman's symbol
}

// Function to clear Pacman's previous cell in the grid
void clearPacmanPreviousCell() {
	grid[pacmanY][pacmanX] = emptySymbol;  // Set the previous cell to empty
}

// Function to set Pacman's current position in the grid
void setPacmanPosition() {
	grid[pacmanY][pacmanX] = pacmanSymbol;  // Place Pacman on the grid at the current position
}

// Function to move Pacman in a specified direction
void movePacmanInDirection(int movingX, int movingY, char nextSymbol) {
	// Handle food collection and change state if Pacman eats food or point
	if (isFoodEaten(nextSymbol)) {
		updateScore(prizeOfFood);  // Update the score for food
		collectedAmountOfFood++;  // Increment the food collected counter
		isFrightenedMode = true;  // Activate frightened mode
		isChaseMode = false;  // Deactivate chase mode
		frightenedModeCount = 0; //reset count
	}
	else if (isPointEaten(nextSymbol)) {
		updateScore(prizeOfPoint);  // Update the score for the point
	}
	clearPacmanPreviousCell();  // Clear the previous position of Pacman
	erasePacmanFromOldPosition();  // Erase Pacman from the old position
	pacmanY += movingY;  // Update Pacman's Y position based on direction
	pacmanX += movingX;  // Update Pacman's X position based on direction
	setPacmanPosition();  // Set Pacman's new position on the grid
	updatePacmanGridPosition();  // Update the grid with the new position
}

// Function to move Pacman up if there is no wall
void movePacmanUp() {
	if (grid[pacmanY - 1][pacmanX] != wallSymbol) {  // Check if the next cell is not a wall
		char nextSymbol = grid[pacmanY - 1][pacmanX];  // Get the symbol of the next cell
		movePacmanInDirection(0, -1, nextSymbol);  // Move Pacman up
	}
}

// Function to move Pacman down if there is no wall
void movePacmanDown() {
	if (grid[pacmanY + 1][pacmanX] != wallSymbol) {  // Check if the next cell is not a wall
		char nextSymbol = grid[pacmanY + 1][pacmanX];  // Get the symbol of the next cell
		movePacmanInDirection(0, 1, nextSymbol);  // Move Pacman down
	}
}

// Function to move Pacman left if there is no wall
void movePacmanLeft() {
	if (grid[pacmanY][pacmanX - 1] != wallSymbol) {  // Check if the next cell is not a wall
		char nextSymbol = grid[pacmanY][pacmanX - 1];  // Get the symbol of the next cell
		movePacmanInDirection(-1, 0, nextSymbol);  // Move Pacman left
	}
}

// Function to move Pacman right if there is no wall
void movePacmanRight() {
	if (grid[pacmanY][pacmanX + 1] != wallSymbol) {  // Check if the next cell is not a wall
		char nextSymbol = grid[pacmanY][pacmanX + 1];  // Get the symbol of the next cell
		movePacmanInDirection(1, 0, nextSymbol);  // Move Pacman right
	}
}

// Function to handle Pacman's movement based on key press (WASD keys)
void handlePacmanMovement() {
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

// Function to get the Y position of a specific ghost
int getGhostYPosition(int ghostNumber) {
	return ghostY[ghostNumber];  // Return the Y coordinate of the specified ghost
}

// Function to get the X position of a specific ghost
int getGhostXPosition(int ghostNumber) {
	return ghostX[ghostNumber];  // Return the X coordinate of the specified ghost
}

// Function to set the position of a specific ghost
void setGhostPosition(int x, int y, int ghostNumber) {
	ghostX[ghostNumber] = x;  // Set X position of the ghost
	ghostY[ghostNumber] = y;  // Set Y position of the ghost
}

// Function to repaint a ghost after moving it, changing its color
void repaintGhost(int ghostNumber, const char* color) {
	int x = getGhostXPosition(ghostNumber);  // Get the current X position of the ghost
	int y = getGhostYPosition(ghostNumber);  // Get the current Y position of the ghost

	setConsoleCursorPosition(x * 2, y);  // Position the cursor for the old ghost position
	std::cout << emptySymbol;  // Clear the old position of the ghost

	setConsoleCursorPosition(x * 2, y);  // Position the cursor to repaint the ghost
	std::cout << color << grid[y][x] << endColor;  // Repaint the ghost with the provided color
}

// Function to spawn all ghosts at specific initial positions
void spawnGhost() {
	// Set initial positions of all ghosts
	setGhostPosition(1, 2, blinkyNumber);  // Blinky
	setGhostPosition(1, 1, pinkyNumber);  // Pinky
	setGhostPosition(2, 1, inkyNumber);   // Inky
	setGhostPosition(3, 1, clydeNumber);  // Clyde

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
void eraseGhostFromOldPosition(int ghostNumber) {
	int x = getGhostXPosition(ghostNumber);  // Get X position of the ghost
	int y = getGhostYPosition(ghostNumber);  // Get Y position of the ghost
	char previousSymbol = lastSymbol[ghostNumber];  // Get the last symbol at the ghost's position

	// Restore the previous symbol (food, point, or empty) in the grid at the ghost's old position
	if (previousSymbol == foodSymbol) {
		grid[y][x] = foodSymbol;
		setConsoleCursorPosition(x * 2, y);  // Position the cursor to the old ghost position
		std::cout << yellowColor << grid[y][x] << endColor;  // Repaint food
	}
	else if (previousSymbol == pointSymbol) {
		grid[y][x] = pointSymbol;
		setConsoleCursorPosition(x * 2, y);
		std::cout << grid[y][x];  // Repaint point
	}
	else if (previousSymbol == emptySymbol) {
		grid[y][x] = emptySymbol;
		setConsoleCursorPosition(x * 2, y);
		std::cout << grid[y][x];  // Repaint empty space
	}
}

// Function to move a ghost to a new position and repaint it with its color
void moveGhostTo(int x, int y, int ghostNumber, char symbol, const char* color) {
	eraseGhostFromOldPosition(ghostNumber);  // Erase the ghost from its old position
	setGhostPosition(x, y, ghostNumber);  // Set the new position of the ghost
	lastSymbol[ghostNumber] = grid[y][x];  // Store the symbol of the new position
	grid[y][x] = symbol;  // Set the ghost symbol at the new position
	repaintGhost(ghostNumber, color);  // Repaint the ghost with its color
}

// Function to check if a given position collides with any ghost
bool isCollidedWithGhost(int x, int y) {
	// Loop through all ghosts and check if any of them occupy the given position
	for (size_t i = 0; i < amountOfGhosts; i++) {
		if (x == ghostX[i] && y == ghostY[i]) {
			return true;  // Return true if collision occurs
		}
	}
	return false;  // Return false if no collision occurs
}

// Function to check if moving up is clear for a ghost (no wall or collision)
bool isDirectionUpClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY - 1][ghostCurX] != wallSymbol  // No wall above
		&& ((ghostCurY - 1) != lastY || ghostCurX != lastX)  // Not the previous position
		&& !isCollidedWithGhost(ghostCurX, ghostCurY - 1));  // No ghost collision above
}

// Function to check if moving left is clear for a ghost (no wall or collision)
bool isDirectionLeftClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY][ghostCurX - 1] != wallSymbol  // No wall to the left
		&& (ghostCurY != lastY || (ghostCurX - 1) != lastX)  // Not the previous position
		&& !isCollidedWithGhost(ghostCurX - 1, ghostCurY));  // No ghost collision to the left
}

// Function to check if moving right is clear for a ghost (no wall or collision)
bool isDirectionRightClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY][ghostCurX + 1] != wallSymbol  // No wall to the right
		&& ((ghostCurY) != lastY || (ghostCurX + 1) != lastX)  // Not the previous position
		&& !isCollidedWithGhost(ghostCurX + 1, ghostCurY));  // No ghost collision to the right
}

// Function to check if moving down is clear for a ghost (no wall or collision)
bool isDirectionDownClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY + 1][ghostCurX] != wallSymbol  // No wall below
		&& ((ghostCurY + 1) != lastY || (ghostCurX) != lastX)  // Not the previous position
		&& !isCollidedWithGhost(ghostCurX, ghostCurY + 1));  // No ghost collision below
}

// Function to calculate the Manhattan distance between the ghost and the predicted position of Pac-Man
int distanceToPacman(int ghostCurX, int ghostCurY, int changeX, int changeY) {
	// Manhattan distance formula: sum of the absolute differences of the X and Y coordinates
	return abs(ghostCurX - (pacmanX + changeX)) + abs(ghostCurY - (pacmanY + changeY));
}

// Function to check if moving backwards is possible for a ghost based on its current position
bool isBackwardsPossible(int x, int y) {
	// make sure that the coords given are in bounds
	if (x > 0 && y > 0 && x < 50 && y < 50) {
		// Check if the grid position is either a point or empty space, allowing the ghost to move backwards
		return (grid[y][x] == pointSymbol || grid[y][x] == emptySymbol);
	}
	return false;
}

// Moves the Red Ghost (Blinky) backwards to its previous position if the move is possible
void moveRedBackwards(const char* color) {
	int curX = getGhostXPosition(blinkyNumber);  // Get the current X position of Blinky
	int curY = getGhostYPosition(blinkyNumber);  // Get the current Y position of Blinky
	if (isBackwardsPossible(redLastX, redLastY)) {  // If the position is valid for moving backwards
		moveGhostTo(redLastX, redLastY, blinkyNumber, blinkySymbol, color);  // Move Blinky backwards
		redLastX = curX;  // Update Blinky's last known X position
		redLastY = curY;  // Update Blinky's last known Y position
	}
}

// Moves the Pink Ghost (Pinky) backwards to its previous position if the move is possible
void movePinkBackwards(const char* color) {
	int curX = getGhostXPosition(pinkyNumber);  // Get the current X position of Pinky
	int curY = getGhostYPosition(pinkyNumber);  // Get the current Y position of Pinky
	if (isBackwardsPossible(pinkLastX, pinkLastY)) {  // If the position is valid for moving backwards
		moveGhostTo(pinkLastX, pinkLastY, pinkyNumber, pinkySymbol, color);  // Move Pinky backwards
		pinkLastX = curX;  // Update Pinky's last known X position
		pinkLastY = curY;  // Update Pinky's last known Y position
	}
}

// Moves the Blue Ghost (Inky) backwards to its previous position if the move is possible
void moveBlueBackwards(const char* color) {
	int curX = getGhostXPosition(inkyNumber);  // Get the current X position of Inky
	int curY = getGhostYPosition(inkyNumber);  // Get the current Y position of Inky
	if (isBackwardsPossible(blueLastX, blueLastY)) {  // If the position is valid for moving backwards
		moveGhostTo(blueLastX, blueLastY, inkyNumber, inkySymbol, color);  // Move Inky backwards
		blueLastX = curX;  // Update Inky's last known X position
		blueLastY = curY;  // Update Inky's last known Y position
	}
}

// Moves the Green Ghost (Clyde) backwards to its previous position if the move is possible
void moveGreenBackwards(const char* color) {
	int curX = getGhostXPosition(clydeNumber);  // Get the current X position of Clyde
	int curY = getGhostYPosition(clydeNumber);  // Get the current Y position of Clyde
	if (isBackwardsPossible(greenLastX, greenLastY)) {  // If the position is valid for moving backwards
		moveGhostTo(greenLastX, greenLastY, clydeNumber, clydeSymbol, color);  // Move Clyde backwards
		greenLastX = curX;  // Update Clyde's last known X position
		greenLastY = curY;  // Update Clyde's last known Y position
	}
}

// Function to make all ghosts move backwards if their respective score conditions are met
void makeAllBackwardsMove() {//used in frightened mode first move
	// If the player's score meets the thresholds, move the corresponding ghost backwards
	if (playerScore >= scoreToActivtRed) {
		if (isBackwardsPossible(redLastX, redLastY))
		{
			moveRedBackwards(blueColor);
		}
	}
	if (playerScore >= scoreToActivtPink) {
		if (isBackwardsPossible(pinkLastX, pinkLastY)) {
			movePinkBackwards(blueColor);
		}
	}
	if (playerScore >= scoreToActivtBlue) {
		if (isBackwardsPossible(blueLastX, blueLastY)) {
			moveBlueBackwards(blueColor);
		}
	}
	if (playerScore >= scoreToActivtGreen) {
		if (isBackwardsPossible(greenLastX, greenLastY)) {
			moveGreenBackwards(blueColor);
		}
	}
}

// Function to move a specific ghost backwards based on its current number (e.g., Blinky, Pinky, etc.)
void moveGhostBack(int curGhostNumber, const char* color) {
	if (curGhostNumber == blinkyNumber) {
		moveRedBackwards(color);  // Move Blinky backwards
	}
	else if (curGhostNumber == pinkyNumber) {
		movePinkBackwards(color);  // Move Pinky backwards
	}
	else if (curGhostNumber == inkyNumber) {
		moveBlueBackwards(color);  // Move Inky backwards
	}
	else if (curGhostNumber == clydeNumber) {
		moveGreenBackwards(color);  // Move Clyde backwards
	}
}

// Function to change the position of a ghost based on the shortest path, or if it can't move, move it backwards or stay
void ghostChangePosition(int ghostCurX, int ghostCurY, int distance[4],
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
void checkDirectionsAvailability(int ghostCurX, int ghostCurY, int distance[], int changeX, int changeY, int lastX, int lastY) {
	int noDirectionClear = 4;  // Count the number of directions that are clear

	// Check if each direction (Up, Left, Down, Right) is clear
	if (isDirectionUpClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[0] = distanceToPacman(ghostCurX, ghostCurY - 1, changeX, changeY);  // Calculate the distance for Up
		noDirectionClear--;  // Decrement the count of directions that are clear
	}
	if (isDirectionLeftClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[1] = distanceToPacman(ghostCurX - 1, ghostCurY, changeX, changeY);  // Calculate the distance for Left
		noDirectionClear--;  // Decrement the count of directions that are clear
	}
	if (isDirectionDownClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[2] = distanceToPacman(ghostCurX, ghostCurY + 1, changeX, changeY);  // Calculate the distance for Down
		noDirectionClear--;  // Decrement the count of directions that are clear
	}
	if (isDirectionRightClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[3] = distanceToPacman(ghostCurX + 1, ghostCurY, changeX, changeY);  // Calculate the distance for Right
		noDirectionClear--;  // Decrement the count of directions that are clear
	}

	// If no direction is clear, set cantMove flag to true
	if (noDirectionClear == 0) {
		cantMove = true;
	}
}

// Function to check if a ghost has been eaten by Pac-Man by checking their positions
int isGhostEatenByPacman() {
	int index = -1;
	for (int i = 0; i < amountOfGhosts; i++) {
		if (getGhostXPosition(i) == pacmanX && getGhostYPosition(i) == pacmanY) {
			return i;  // Return the index of the ghost that was eaten
		}
	}
	return index;  // Return -1 if no ghost is eaten
}

// Function to check if a corner is available for a ghost to move into
bool isCornerAvailable(int x, int y) {
	return false;  // This function always returns false (not implemented yet)
}

// Function to check if a corner is clear for a ghost to move into
bool isCornerClear(int x, int y) {
	// Check if the grid position is either a point or empty space, allowing the ghost to move into the corner
	return (grid[y][x] == pointSymbol) || (grid[y][x] == emptySymbol);
}

// Function to send a ghost to a corner on the grid
void sendGhostToCorner(int ghostNumber) {
	int curHeightOfGrid = heightGrid - wallsIncl;
	int curWidthOfGrid = widthGrid - wallsIncl;

	switch (ghostNumber) {
	case 0:
		if (isCornerClear(1, curWidthOfGrid - 1)) {
			moveGhostTo(1, curWidthOfGrid - 1, 0, blinkyNumber, redColor);  // Move Blinky to corner
		}
		else if (isCornerClear(2, curWidthOfGrid - 1)) {
			moveGhostTo(2, curWidthOfGrid - 1, 0, blinkyNumber, redColor);  // Move Blinky to adjacent position
		}
		break;
	case 1:
		if (isCornerClear(1, 1)) {
			moveGhostTo(1, 1, 1, pinkyNumber, pinkColor);  // Move Pinky to corner
		}
		else if (isCornerClear(1, 2)) {
			moveGhostTo(1, 2, 1, pinkyNumber, pinkColor);  // Move Pinky to adjacent position
		}
		break;
	case 2:
		if (isCornerClear(curHeightOfGrid - 1, curWidthOfGrid - 1)) {
			moveGhostTo(curHeightOfGrid - 1, curWidthOfGrid - 1, 2, inkyNumber, blueColor);  // Move Inky to corner
		}
		else if (isCornerClear(curHeightOfGrid - 2, curWidthOfGrid - 1)) {
			moveGhostTo(curHeightOfGrid - 2, curWidthOfGrid - 1, 2, inkyNumber, blueColor);  // Move Inky to adjacent position
		}
		break;
	case 3:
		if (isCornerClear(curHeightOfGrid - 1, 1)) {
			moveGhostTo(curHeightOfGrid - 1, 1, 3, clydeNumber, greenColor);  // Move Clyde to corner
		}
		else if (isCornerClear(curHeightOfGrid - 2, 1)) {
			moveGhostTo(curHeightOfGrid - 2, 1, 3, clydeNumber, greenColor);  // Move Clyde to adjacent position
		}
		break;
	}
}

// Function to activate ghosts based on the player's score
int activateReadyGhosts() {
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

// Function to move ghosts randomly within the grid based on available directions
void randomGhostMoves(int ghostCurX, int ghostCurY, int lastX, int lastY, int ghostNumber, char ghostSymbol, const char* color) {
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
		return; // Return to prevent random movement if no directions are valid
	}

	// Otherwise, move in a random valid direction
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

	// Update the last position of the ghost after moving
	lastX = ghostCurX;
	lastY = ghostCurY;
}

//function that activates the frigthened mode
void frightenedModeActivated(int ghostCurX, int ghostCurY, int lastX, int lastY, int ghostNumber, char ghostSymbol, const char* color) {
	randomGhostMoves(ghostCurX, ghostCurY, lastX, lastY, ghostNumber, ghostSymbol, color);
	repaintGhost(ghostNumber, blueColor);
}

//iterates through the ghosts that are awake and turns on the their frightened Mode
void frightenedModeGhosts() {
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

//red algorithm
void shortestPathAlgorithmRed() {
	int ghostCurX = getGhostXPosition(blinkyNumber);
	int ghostCurY = getGhostYPosition(blinkyNumber);
	int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };
	checkDirectionsAvailability(ghostCurX, ghostCurY, distance, 0, 0, redLastX, redLastY);//checks available directions for the ghost
	//changes to the available position or stays at the same place and waits
	ghostChangePosition(ghostCurX, ghostCurY, distance, blinkyNumber, blinkySymbol, redColor, redLastX, redLastY);

}

/// Pink Ghost algorithm
void shortestPathAlgorithmPink() {
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

// Blue Ghost algorithm
void shortestPathAlgorithmBlue() {
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

	// Calculate the direction vector between Red ghost and Pacman’s target position
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
// Green Ghost algorithm (Clyde)
void shortestPathAlgorithmGreen() {
	// Get the current position of the Green ghost (Clyde)
	int ghostCurX = getGhostXPosition(clydeNumber);
	int ghostCurY = getGhostYPosition(clydeNumber);

	// If Clyde is less than 8 tiles away from Pacman, pursue Pacman
	if (distanceToPacman(ghostCurX, ghostCurY, 0, 0) < 8) {
		// Create an array to store the distance for each direction (up, down, left, right)
		int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };

		// Check available directions and calculate distances for Clyde's move towards Pacman (target = (0, 0))
		checkDirectionsAvailability(ghostCurX, ghostCurY, distance, 0, 0, greenLastX, greenLastY);

		// Move Clyde based on the smallest distance (best direction towards Pacman)
		ghostChangePosition(ghostCurX, ghostCurY, distance, clydeNumber, clydeSymbol, greenColor, greenLastX, greenLastY);
	}
	// If Clyde is 8 or more tiles away from Pacman, switch to scatter mode (move towards the bottom-left corner)
	else if (distanceToPacman(ghostCurX, ghostCurY, 0, 0) >= 8) {
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


// Activate the Red Ghost (Blinky)
void activateRedGhost() {
	shortestPathAlgorithmRed();  // Call the function to move the Red Ghost based on the shortest path algorithm
}

// Activate the Pink Ghost (Pinky)
void activatePinkGhost() {
	shortestPathAlgorithmPink();  // Call the function to move the Pink Ghost based on the shortest path algorithm
}

// Activate the Blue Ghost (Inky)
void activateBlueGhost() {
	shortestPathAlgorithmBlue();  // Call the function to move the Blue Ghost based on the shortest path algorithm
}

// Activate the Green Ghost (Clyde)
void activateGreenGhost() {
	shortestPathAlgorithmGreen();  // Call the function to move the Green Ghost based on the shortest path algorithm
}

// Reset the movement direction of Pacman (used after each mode change)
void pacmanMovmentDirectionReset() {
	isGoingDown = false;
	isGoingUp = false;
	isGoingRight = false;
	isGoingLeft = false;
}

// Activate ghosts based on the player's score
void activateGhosts() {
	// Check the player's score and activate ghosts accordingly
	if (playerScore >= scoreToActivtRed) {
		activateRedGhost();  // Activate Red Ghost
	}
	if (playerScore >= scoreToActivtPink) {
		activatePinkGhost();  // Activate Pink Ghost
	}
	if (playerScore >= scoreToActivtBlue) {
		activateBlueGhost();  // Activate Blue Ghost
	}
	if (playerScore >= scoreToActivtGreen) {
		activateGreenGhost();  // Activate Green Ghost
	}
	// Reset Pacman's movement direction after activating ghosts
	pacmanMovmentDirectionReset();
}

// Call chase mode, activating the ghosts and handling Pacman's movement
void callChaseMode() {
	activateGhosts();  // Activate ghosts based on the current score
	handlePacmanMovement();  // Handle Pacman's movement
	displayPlayerScore();  // Display the current player score
}

// Call frightened mode, which either makes ghosts move backward or perform normal frightened behavior
void callFrightenedMode() {
	if (frightenedModeCount == 0) {
		makeAllBackwardsMove();  // Make ghosts move backward if the frightened mode count is 0
	}
	else {
		frightenedModeGhosts();  // Otherwise, apply frightened mode behavior for ghosts
	}
	handlePacmanMovement();  // Handle Pacman's movement during frightened mode
	displayPlayerScore();  // Display the current player score
}

// End the frightened mode, resetting the state to chase mode
void endFrightenedMode() {
	isFrightenedMode = false;  // Disable frightened mode
	isChaseMode = true;  // Enable chase mode
	frightenedModeCount = 0;  // Reset the frightened mode count
}

// Main game loop that keeps running until the game is over
void runGameLoop() {
	// Game continues as long as the game is not over
	while (!isGameOver()) {
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
			if (frightenedModeCount == 20) {
				endFrightenedMode();
			}
		}
	}
	Sleep(600);  // After game ends, sleep for a short time before displaying the game over screen
	showGameOverScreen();  // Show the game over screen
}

// Initialize the game environment (grid, map, ghosts, etc.)
void InitializeGame() {
	hideConsoleCursor();  // Hide the console cursor to avoid cluttering the screen
	srand((unsigned int)time(0));  // Seed the random number generator for randomness
	initializeGridDimensions();  // Set up grid dimensions for the game
	loadMapFromFile();  // Load the map from a file (where the game layout is stored)
	renderGrid();  // Render the grid (game area) on the screen
	spawnGhost();  // Spawn the ghosts at their initial positions
	spawnPacman();  // Spawn Pacman at the starting position
	spawnFood();  // Spawn food on the map
}

// Start the Pacman game, initializing it and then running the game loop
void startPacmanGame() {
	InitializeGame();  // Initialize the game environment
	Sleep(3000);  // Wait for 3 seconds before starting the game (to give the player time to prepare)
	runGameLoop();  // Start the main game loop
}

// The main entry point of the game
int main() {
	startPacmanGame();  // Start the Pacman game
	return 0;  // Exit the program
}