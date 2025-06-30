#include "Console.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include "Settings.h"

std::ifstream file("map.txt");
// Function to set the cursor position at a specific (x, y) coordinate in the console window
void Console::setConsoleCursorPosition(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to the console output
	COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };  // Create a COORD structure to store the position
	SetConsoleCursorPosition(hConsole, position);  // Set the cursor position to the specified (x, y)
}

// Function to hide the console cursor (makes it invisible)
void Console::hideConsoleCursor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to the console output
	CONSOLE_CURSOR_INFO cursorInfo;  // Declare a CONSOLE_CURSOR_INFO structure to hold cursor visibility information
	cursorInfo.dwSize = 1;  // Set the cursor size (a smaller value makes the cursor "invisible")
	cursorInfo.bVisible = FALSE;  // Set the cursor visibility to false (hide it)
	SetConsoleCursorInfo(hConsole, &cursorInfo);  // Apply the cursor settings
}

// Function to clear the console screen (removes all output on the console)
void Console::clearConsole() {
	system("cls");  // Execute the system command "cls" to clear the console
}

// Function to resize the console window (set its height and width)
void Console::resizeConsole(int height, int width) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to the console output
	// Set the window size using a SMALL_RECT structure, which defines the window's top-left and bottom-right corners
	SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(width * 2 + 15), static_cast<SHORT>(height + 1) };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);  // Apply the new window size
}

// Function to center the console window on the screen
void Console::centerConsole() {
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
void Console::initializeGridMemory(int rows, int cols) {
	grid = new char* [rows];  // Allocate memory for rows
	for (int i = 0; i < rows; i++) {
		grid[i] = new char[cols];  // Allocate memory for each row
		for (int j = 0; j < cols; j++) {
			grid[i][j] = ' ';  // Initialize all cells with empty spaces
		}
	}
}

// Function to free dynamically allocated memory for the grid
void Console::freeGridMemory() {
	if (grid != nullptr) {
		for (int i = 0; i < heightGrid; i++) {
			delete[] grid[i];  // Free memory for each row
		}
		delete[] grid;  // Free memory for the grid itself
		grid = nullptr;  // Set the grid pointer to null to avoid dangling reference
	}
}

// Function to skip a line in the file
void Console::skipLineInFile() {
	file.ignore(std::streamsize(INT_MAX), '\n');  // Skip characters until newline
}

// Function to add boundary walls around the grid
void Console::addBoundaryWalls() {
	for (size_t i = 0; i < heightGrid; i++) {
		for (size_t j = 0; j < widthGrid; j++) {
			if ((heightGrid - 1) == i || (widthGrid - 1) == j) {
				grid[i][j] = '#';  // Set boundary walls
			}
		}
	}
}

// Function to load a map from a file into the grid
void Console::loadMapFromFile() {
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
void Console::renderGrid() {
	for (int i = 0; i < heightGrid; ++i) {
		for (int j = 0; j < widthGrid; ++j) {
			std::cout << grid[i][j] << " ";  // Print each cell of the grid
		}
		std::cout << std::endl;  // Move to the next line after a row
	}
}

// Function to prepare the console window for the game
void Console::prepareConsoleForGame() {
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
void Console::initializeGridDimensions() {
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
		if (widthGrid < 49) {
			widthGrid += wallsIncl;
		}
		if (heightGrid < 49) {
			heightGrid += wallsIncl;
		}
		if (widthGrid == 49) {
			widthGrid += wallsIncl - 1;
		}
		if (heightGrid == 49) {
			heightGrid += wallsIncl - 1;
		}


		break;  // Exit the loop if valid dimensions are entered
	}
	prepareConsoleForGame();  // Prepare the console for the game with the initialized dimensions
}

// Function to wait for any key press to exit the game
void Console::waitForGameExit() {
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
void Console::displayPlayerScore() {
	setConsoleCursorPosition(widthGrid * 2 + 1, heightGrid / 2);  // Set cursor position for score display
	std::cout << "Score: " << playerScore << "  ";  // Display the player's score
}


// Function to update the player's score by a specified number of points
void Console::updateScore(int points) {
	playerScore += points;  // Update the score
	Console::displayPlayerScore();  // Display the updated score
}


//screen is shown when the game is lost
void Console::showLoseScreen() {
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
void Console::showWinScreen() {
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