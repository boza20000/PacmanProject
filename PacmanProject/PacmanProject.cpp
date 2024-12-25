#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h> 
#include <fstream>

int pacmanX, pacmanY;
size_t playerScore = 0;

const char pacmanSymbol = 'P';
const char wallSymbol = '#';
const char foodSymbol = '@';
const char pointSymbol = '.';
const char emptySymbol = ' ';

const size_t amountOfGhosts = 4;
int ghostX[amountOfGhosts], ghostY[amountOfGhosts];
const char blinkySymbol = 'B';
const char pinkySymbol = 'Y';
const char inkySymbol = 'I';
const char clydeSymbol = 'C';

const size_t blinkyNumber = 0;
const size_t pinkyNumber = 1;
const size_t inkyNumber = 2;
const size_t clydeNumber = 3;

const char* redColor = "\033[31m";
const char* pinkColor = "\033[35m";
const char* blueColor = "\033[34m";
const char* greenColor = "\033[32m";
const char* endColor = "\033[0m";

const size_t scoreToActivtRed = 0;
const size_t scoreToActivtPink = 20;
const size_t scoreToActivtBlue = 40;
const size_t scoreToActivtGreen = 60;

const size_t foodAmount = 3;
const size_t prizeOfFood = 20;
const size_t prizeOfPoint = 2;

int foodX[foodAmount], foodY[foodAmount];
bool isGhostRemovable = false;
bool isNormalMode = true;
bool isChaseMode = false;
const size_t wallsIncl = 2;
size_t redLastX = -1;
size_t redLastY = -1;
char lastSymbol = ' ';
char** grid = nullptr;
int widthGrid = 0, heightGrid = 0;
std::ifstream file("map.txt");

void setConsoleCursorPosition(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
	SetConsoleCursorPosition(hConsole, position);
}

void hideConsoleCursor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void clearConsole() {
	system("cls");
}

void resizeConsole(int height, int width) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(width * 2 + 15), static_cast<SHORT>(height + 1) };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

void centerConsole() {
	RECT screenRect;
	GetWindowRect(GetDesktopWindow(), &screenRect); // Get the desktop window dimensions
	int screenWidth = screenRect.right - screenRect.left;
	int screenHeight = screenRect.bottom - screenRect.top;

	// Step 2: Get the current console window handle
	HWND consoleWindow = GetConsoleWindow();

	// Step 3: Get the console window's size (width and height)
	RECT consoleRect;
	GetWindowRect(consoleWindow, &consoleRect);
	int consoleWidth = consoleRect.right - consoleRect.left;
	int consoleHeight = consoleRect.bottom - consoleRect.top;

	// Step 4: Calculate the position to center the console window
	int xPos = (screenWidth - consoleWidth) / 2 - 10;
	int yPos = (screenHeight - consoleHeight) / 2;

	// Step 5: Move the console window to the calculated position
	MoveWindow(consoleWindow, xPos, yPos, consoleWidth, consoleHeight, TRUE);
}

void initializeGridMemory(int rows, int cols) {
	grid = new char* [rows];
	for (int i = 0; i < rows; i++) {
		grid[i] = new char[cols];
		for (int j = 0; j < cols; j++) {
			grid[i][j] = ' ';
		}
	}
}

void freeGridMemory() {
	if (grid != nullptr) {
		for (int i = 0; i < heightGrid; i++) {
			delete[] grid[i];
		}
		delete[] grid;
		grid = nullptr;
	}
}

void skipLineInFile() {
	file.ignore(std::streamsize(INT_MAX), '\n'); // Skip to the next line
}

//put walls over the sides that are cut 
void addBoundaryWalls() {

	for (size_t i = 0; i < heightGrid; i++)
	{
		for (size_t j = 0; j < widthGrid; j++)
		{
			if ((heightGrid - 1) == i || (widthGrid - 1) == j) {
				grid[i][j] = '#';
			}
		}
	}

}

//gest the map from a file with the height and width that the user inputed
void loadMapFromFile() {
	if (!file) {
		std::cerr << "Error: Could not open file" << std::endl;
		return;
	}
	for (int row = 0; row < heightGrid; row++) {
		for (int col = 0; col < widthGrid; col++) {
			char ch;
			if (file.get(ch)) {
				grid[row][col] = ch;
			}
		}
		skipLineInFile();
	}
	file.close();

	addBoundaryWalls();
}

//displayes the map 
void renderGrid() {
	for (int i = 0; i < heightGrid; ++i) {
		for (int j = 0; j < widthGrid; ++j) {
			std::cout << grid[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void prepareConsoleForGame() {
	if (heightGrid < 20 || widthGrid < 40) {
		resizeConsole(heightGrid + 20, widthGrid + 40);
	}
	else {
		resizeConsole(heightGrid, widthGrid);
	}
	centerConsole();
	initializeGridMemory(heightGrid, widthGrid);
	clearConsole();
}

//starts the game and waits for grid paremeters
void initializeGridDimensions() {
	std::cout << "Grid dimensions must be positive integers over 10 under 50(incl.)\nwith difference betwen the parameters less than 10 " << std::endl << std::endl;
	std::cout << "Enter grid width: ";
	std::cin >> widthGrid;
	std::cout << "Enter grid height: ";
	std::cin >> heightGrid;

	int diff = std::abs(widthGrid - heightGrid);
	if (widthGrid < 10 || heightGrid < 10 || widthGrid > 50 || heightGrid > 50 || diff > 10) {
		std::cerr << "Error: Grid dimensions must be positive integers over 10 under 50(incl.)\nwith difference betwen the parameters less than 10 " << std::endl;
		std::exit(1);
	}

	//handles the case where no bounderies requiered
	if (widthGrid < 49 || heightGrid < 49) {
		widthGrid += wallsIncl;
		heightGrid += wallsIncl;
	}

	//prepares the basic requirments for the console to look good for the game
	prepareConsoleForGame();
}

//waits for keystrock to end the game
void waitForGameExit() {
	while (true) {
		for (int key = 0x08; key <= 0xFE; ++key) {
			if (GetAsyncKeyState(key) & 0x8000) {
				clearConsole();
				exit(0);
			}
		}
		Sleep(50);
	}
}

//visualises the player score
void displayPlayerScore() {
	setConsoleCursorPosition(widthGrid * 2 + 1, heightGrid / 2);
	std::cout << "Score: " << playerScore << "  ";
}

//updates score of the player
void updateScore(int points) {
	playerScore += points;
	displayPlayerScore();
}

//game over screen
void showGameOverScreen() {
	clearConsole();
	std::cout << R"(
  #####     #    #     # ######      ######  #          # ###### ######      #
 #     #   # #   ##   ## #          #      #  #        #  #      #     #     # 
 #        #   #  # # # # #          #      #   #      #   #      #     #     #
 #  #### #     # #  #  # ######     #      #    #    #    ###### #    #      #
 #     # ####### #     # #          #      #     #  #     #      ####        #
 #     # #     # #     # #          #      #     #  #     #      #   #        
  #####  #     # #     # ######      ######       ##      ###### #     #     #
    )" << std::endl;

	std::cout << "\n\n          Your Final Score: " << playerScore << "\n\n";

	std::cout << "   Press any key to exit the game...";
	waitForGameExit();
}

//check whether all food is collected
bool isAllFoodCollected() {
	for (size_t i = 0; i < foodAmount; i++) {
		if (grid[foodY[i]][foodX[i]] == foodSymbol) {
			return false;
		}
	}
	return true;
}

//check for collision with ghost
bool isPacmanCaughtByGhost() {
	for (size_t i = 0; i < amountOfGhosts; i++)
	{
		if (ghostX[i] == pacmanX && ghostY[i] == pacmanY)
		{
			return  true;
		}
	}
	return false;
}

//the game is not over untill the player collects the foods or collides with a ghost
bool isGameOver() {
	return isPacmanCaughtByGhost() || isAllFoodCollected();
}

void enableFrightenedMode() {
	isGhostRemovable = true;
}
void spawnFood() {
	for (int i = 0; i < foodAmount; i++) {
		int foodYCur = 0;
		int foodXCur = 0;
		do {
			foodYCur = rand() % (heightGrid - 2) + 1;
			foodXCur = rand() % (widthGrid - 2) + 1;
		} while (grid[foodYCur][foodXCur] != pointSymbol);
		foodX[i] = foodXCur;
		foodY[i] = foodYCur;
		grid[foodYCur][foodXCur] = foodSymbol;
		setConsoleCursorPosition(foodXCur * 2, foodYCur);
		std::cout << foodSymbol;
	}
}

bool isFoodEaten(char ch) {
	return ch == foodSymbol;
}
bool isPointEaten(char ch) {
	return ch == pointSymbol;
}

void setScreenSize() {
	//create a function that handels big matrix (consol handles to 30x30)
}


void spawnPacman() {
	do {
		pacmanX = rand() % (widthGrid - 2) + 1;
		pacmanY = rand() % (heightGrid - 2) + 1;
	} while (grid[pacmanY][pacmanX] != pointSymbol);
	grid[pacmanY][pacmanX] = pacmanSymbol;
	setConsoleCursorPosition(pacmanX * 2, pacmanY);  // Adjust cursor for spacing
	std::cout << pacmanSymbol;
}

void updateGridCell(int x, int y, char symbol) {
	setConsoleCursorPosition(x * 2, y);  // Adjust cursor for grid spacing
	std::cout << symbol;  // Update only the specified cell
}
void erasePacmanFromOldPosition() {
	updateGridCell(pacmanX, pacmanY, emptySymbol);
}

void updatePacmanGridPosition() {
	updateGridCell(pacmanX, pacmanY, pacmanSymbol);
}

void clearPacmanPreviousCell() {
	grid[pacmanY][pacmanX] = emptySymbol;
}

void setPacmanPosition() {
	grid[pacmanY][pacmanX] = pacmanSymbol;
}


void movePacmanInDirection(int movingX, int movingY, char nextSymbol) {
	if (isFoodEaten(nextSymbol)) {
		updateScore(prizeOfFood);
	}
	else if (isPointEaten(nextSymbol)) {
		updateScore(prizeOfPoint);
	}
	clearPacmanPreviousCell();
	erasePacmanFromOldPosition();
	pacmanY += movingY;
	pacmanX += movingX;
	setPacmanPosition();
	updatePacmanGridPosition();
}


void movePacmanUp() {
	if (grid[pacmanY - 1][pacmanX] != wallSymbol) {
		char nextSymbol = grid[pacmanY - 1][pacmanX];
		movePacmanInDirection(0, -1, nextSymbol);
	}
}


void movePacmanDown() {
	if (grid[pacmanY + 1][pacmanX] != wallSymbol) {
		char nextSymbol = grid[pacmanY + 1][pacmanX];
		movePacmanInDirection(0, 1, nextSymbol);

	}
}

void movePacmanLeft() {
	if (grid[pacmanY][pacmanX - 1] != wallSymbol) {
		char nextSymbol = grid[pacmanY][pacmanX - 1];
		movePacmanInDirection(-1, 0, nextSymbol);
	}
}

void movePacmanRight() {
	if (grid[pacmanY][pacmanX + 1] != wallSymbol) {
		char nextSymbol = grid[pacmanY][pacmanX + 1];
		movePacmanInDirection(1, 0, nextSymbol);
	}
}

void handlePacmanMovement() {
	const int KEY_PRESSED_MASK = 0x8000;
	if (GetAsyncKeyState('W') & KEY_PRESSED_MASK) {  // W key pressed (up)
		movePacmanUp();
	}
	if (GetAsyncKeyState('S') & KEY_PRESSED_MASK) {  // S key pressed (down)
		movePacmanDown();
	}
	if (GetAsyncKeyState('A') & KEY_PRESSED_MASK) {  // A key pressed (left)
		movePacmanLeft();
	}
	if (GetAsyncKeyState('D') & KEY_PRESSED_MASK) {  // D key pressed (right)
		movePacmanRight();
	}
}

int getGhostYPosition(int ghostNumber) {
	return ghostY[ghostNumber];
}

int getGhostXPosition(int ghostNumber) {
	return ghostX[ghostNumber];
}

void setGhostPosition(int x, int y, int ghostNumber) {
	ghostX[ghostNumber] = x;
	ghostY[ghostNumber] = y;
}

void repaintGhost(int ghostNumber, const char* color) {
	int x = getGhostXPosition(ghostNumber);
	int y = getGhostYPosition(ghostNumber);

	setConsoleCursorPosition(x * 2, y);
	std::cout << emptySymbol;

	setConsoleCursorPosition(x * 2, y);
	std::cout << color << grid[y][x] << endColor;
}

void spawnGhost() {

	//starting coords
	//put in array for ghosts
	setGhostPosition(1, 2, blinkyNumber);  // Blinky
	setGhostPosition(1, 1, pinkyNumber);  // Pinky
	setGhostPosition(2, 1, inkyNumber);  // Inky
	setGhostPosition(3, 1, clydeNumber);  // Clyde


	grid[ghostY[0]][ghostX[0]] = blinkySymbol;
	grid[ghostY[1]][ghostX[1]] = pinkySymbol;
	grid[ghostY[2]][ghostX[2]] = inkySymbol;
	grid[ghostY[3]][ghostX[3]] = clydeSymbol;

	repaintGhost(blinkyNumber, redColor);  // Red for Blinky
	repaintGhost(pinkyNumber, pinkColor);  // Pink for Pinky
	repaintGhost(inkyNumber, blueColor);  // Blue for Inky
	repaintGhost(clydeNumber, greenColor);  // Green for Clyde
}

void eraseGhostFromOldPosition(int ghostNumber) {
	int x = getGhostXPosition(ghostNumber);
	int y = getGhostYPosition(ghostNumber);

	if (lastSymbol == foodSymbol) {
		grid[y][x] = foodSymbol;
		setConsoleCursorPosition(x * 2, y);
		std::cout << foodSymbol;
	}
	else if (lastSymbol == pointSymbol) {
		grid[y][x] = pointSymbol;
		setConsoleCursorPosition(x * 2, y);
		std::cout << pointSymbol;
	}
	else if (lastSymbol == emptySymbol) {
		grid[y][x] = emptySymbol;
		setConsoleCursorPosition(x * 2, y);
		std::cout << emptySymbol;
	}
}

void moveGhostTo(int x, int y, int ghostNumber, char symbol) {
	eraseGhostFromOldPosition(ghostNumber);
	setGhostPosition(x, y, ghostNumber);
	lastSymbol = grid[y][x];
	grid[y][x] = symbol;
	repaintGhost(ghostNumber, redColor);
}

//cant go back
void shortestPathAlgorithmRed(int ghostNumber) {
	int ghostCurX = getGhostXPosition(ghostNumber);
	int ghostCurY = getGhostYPosition(ghostNumber);

	int nextX = ghostCurX;
	int nextY = ghostCurY;

	int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };

	// Up
	if (grid[ghostCurY - 1][ghostCurX] != wallSymbol && ((ghostCurY - 1) != redLastY || ghostCurX != redLastX)) {
		distance[0] = abs(ghostCurX - pacmanX) + abs((ghostCurY - 1) - pacmanY);
	}
	// Left
	if (grid[ghostCurY][ghostCurX - 1] != wallSymbol && (ghostCurY != redLastY || (ghostCurX - 1) != redLastX)) {
		distance[1] = abs((ghostCurX - 1) - pacmanX) + abs(ghostCurY - pacmanY);
	}
	// Down
	if (grid[ghostCurY + 1][ghostCurX] != wallSymbol && ((ghostCurY + 1) != redLastY || ghostCurX != redLastX)) {
		distance[2] = abs(ghostCurX - pacmanX) + abs((ghostCurY + 1) - pacmanY);
	}
	// Right
	if (grid[ghostCurY][ghostCurX + 1] != wallSymbol && (ghostCurY != redLastY || (ghostCurX + 1) != redLastX)) {
		distance[3] = abs((ghostCurX + 1) - pacmanX) + abs(ghostCurY - pacmanY);
	}

	int minDistance = INT_MAX;
	int direction = -1;

	for (int i = 0; i < 4; i++) {
		if (distance[i] < minDistance) {
			minDistance = distance[i];
			direction = i;
		}
	}

	// Update the next position based on the chosen direction
	switch (direction) {
	case 0: nextY = ghostCurY - 1; break; // Up
	case 1: nextX = ghostCurX - 1; break; // Left
	case 2: nextY = ghostCurY + 1; break; // Down
	case 3: nextX = ghostCurX + 1; break; // Right
	}


	moveGhostTo(nextX, nextY, ghostNumber, blinkySymbol);
	redLastX = ghostCurX;
	redLastY = ghostCurY;
}

void activateRedGhost() {
	shortestPathAlgorithmRed(blinkyNumber);
	//normal search
}
void activatePinkGhost() {
	//shortestPathAlgorithm(pinkyNumber);
	// 4 in front of pacman and 4 tothe left
}
void activateBlueGhost() {
	//shortestPathAlgorithm(inkyNumber);
	// 2 in fron and 2 to  the left from pacman
}
void activateGreenGhost() {
	//shortestPathAlgorithm(clydeNumber);
	//>=8 distance same as Red but <8
}

void activateGhosts() {
	if (playerScore >= scoreToActivtRed) {
		activateRedGhost();
	}
	/*if (playerScore >= scoreToActivtPink) {
		activatePinkGhost();
	}
	if (playerScore >= scoreToActivtBlue) {
		activateBlueGhost();
	}
	if (playerScore >= scoreToActivtGreen) {
		activateGreenGhost();
	}*/
}


void InitializeGame() {
	hideConsoleCursor();
	srand(time(0));
	initializeGridDimensions();
	loadMapFromFile();
	renderGrid();
	spawnGhost();
	spawnPacman();
	spawnFood();
}

void runGameLoop() {
	while (!isGameOver()) {
		activateGhosts();
		handlePacmanMovement();
		displayPlayerScore();
		Sleep(125);
	}
	Sleep(600);
	showGameOverScreen();
}

//the function calls the game
void startPacmanGame() {
	InitializeGame();
	runGameLoop();
}

int main() {
	startPacmanGame();
	return 0;
}