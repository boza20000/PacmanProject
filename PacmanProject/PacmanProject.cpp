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
const char* yellowColor = "\033[33m";
const char* endColor = "\033[0m";

const size_t scoreToActivtRed = 0;
const size_t scoreToActivtPink = 20;
const size_t scoreToActivtBlue = 40;
const size_t scoreToActivtGreen = 60;

const size_t foodAmount = 3;
const size_t prizeOfFood = 20;
const size_t prizeOfPoint = 2;

bool isGoingLeft = false;
bool isGoingRight = false;
bool isGoingUp = false;
bool isGoingDown = false;

int foodX[foodAmount], foodY[foodAmount];
bool isGhostRemovable = false;
bool isNormalMode = true;
bool isChaseMode = false;
const size_t wallsIncl = 2;
int redLastX = -1;
int redLastY = -1;
int pinkLastX = -1;
int pinkLastY = -1;
int blueLastX = -1;
int blueLastY = -1;
char lastSymbol[amountOfGhosts] = { ' ',' ',' ',' ' };
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
	if (heightGrid < 20 || widthGrid < 20) {
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
	std::cout << yellowColor << symbol << endColor;  // Update only the specified cell
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
		isGoingUp = true;
		movePacmanUp();
	}
	if (GetAsyncKeyState('S') & KEY_PRESSED_MASK) {  // S key pressed (down)
		isGoingDown = true;
		movePacmanDown();
	}
	if (GetAsyncKeyState('A') & KEY_PRESSED_MASK) {  // A key pressed (left)
		isGoingLeft = true;
		movePacmanLeft();
	}
	if (GetAsyncKeyState('D') & KEY_PRESSED_MASK) {  // D key pressed (right)
		isGoingRight = true;
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
	char previousSymbol = lastSymbol[ghostNumber];
	if (previousSymbol == foodSymbol) {
		grid[y][x] = foodSymbol;
	}
	else if (previousSymbol == pointSymbol) {
		grid[y][x] = pointSymbol;
	}
	else if (previousSymbol == emptySymbol) {
		grid[y][x] = emptySymbol;
	}
	setConsoleCursorPosition(x * 2, y);
	std::cout << grid[y][x];
}


void moveGhostTo(int x, int y, int ghostNumber, char symbol, const char* color) {
	eraseGhostFromOldPosition(ghostNumber);
	setGhostPosition(x, y, ghostNumber);
	lastSymbol[ghostNumber] = grid[y][x];
	grid[y][x] = symbol;
	repaintGhost(ghostNumber, color);
}
bool isCollidedWithGhost(int x, int y) {
	for (size_t i = 0; i < amountOfGhosts; i++) {
		if (x == ghostX[i] && y == ghostY[i]) {
			return true;
		}
	}
	return false;
}
bool isDirectionUpClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY - 1][ghostCurX] != wallSymbol
		&& ((ghostCurY - 1) != lastY || ghostCurX != lastX)
		&& !isCollidedWithGhost(ghostCurX, ghostCurY - 1));
}
bool isDirectionLeftClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY][ghostCurX - 1] != wallSymbol
		&& (ghostCurY != lastY || (ghostCurX - 1) != lastX)
		&& !isCollidedWithGhost(ghostCurX - 1, ghostCurY));
}
bool isDirectionRightClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY][ghostCurX + 1] != wallSymbol
		&& ((ghostCurY) != lastY || (ghostCurX + 1) != lastX)
		&& !isCollidedWithGhost(ghostCurX + 1, ghostCurY));
}
bool isDirectionDownClear(int ghostCurX, int ghostCurY, int lastX, int lastY) {
	return (grid[ghostCurY + 1][ghostCurX] != wallSymbol
		&& ((ghostCurY + 1) != lastY || (ghostCurX) != lastX)
		&& !isCollidedWithGhost(ghostCurX, ghostCurY + 1));
}

int distanceToPacman(int ghostCurX, int ghostCurY, int changeX, int changeY) {
	return abs(ghostCurX - (pacmanX + changeX)) + abs(ghostCurY - (pacmanY + changeY));
}

void ghostChangePosition(int ghostCurX, int ghostCurY, int nextX, int nextY, int distance[4], int ghostNumber, int symbol, const char* color, int& lastX, int& lastY) {
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

	// Move the ghost to the new position
	moveGhostTo(nextX, nextY, ghostNumber, symbol, color);

	// Update the last position of the ghost
	lastX = ghostCurX;
	lastY = ghostCurY;
}

void checkDirectionsAvailability(int ghostCurX, int ghostCurY, int distance[], int changeX, int changeY, int lastX, int lastY) {
	// Up
	if (isDirectionUpClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[0] = distanceToPacman(ghostCurX, ghostCurY - 1, changeX, changeY);
	}
	// Left
	if (isDirectionLeftClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[1] = distanceToPacman(ghostCurX - 1, ghostCurY, changeX, changeY);
	}
	// Down
	if (isDirectionDownClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[2] = distanceToPacman(ghostCurX, ghostCurY + 1, changeX, changeY);
	}
	// Right
	if (isDirectionRightClear(ghostCurX, ghostCurY, lastX, lastY)) {
		distance[3] = distanceToPacman(ghostCurX + 1, ghostCurY, changeX, changeY);
	}
}
//red algorithm
void shortestPathAlgorithmRed() {
	int ghostCurX = getGhostXPosition(blinkyNumber);
	int ghostCurY = getGhostYPosition(blinkyNumber);
	int nextX = ghostCurX;
	int nextY = ghostCurY;
	int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };
	checkDirectionsAvailability(ghostCurX, ghostCurY, distance, 0, 0, redLastX, redLastY);
	ghostChangePosition(ghostCurX, ghostCurY, nextX, nextY, distance, blinkyNumber, blinkySymbol, redColor, redLastX, redLastY);
}

//pink algorithm
void shortestPathAlgorithmPink() {
	int ghostCurX = getGhostXPosition(pinkyNumber);
	int ghostCurY = getGhostYPosition(pinkyNumber);
	int nextX = ghostCurX;
	int nextY = ghostCurY;
	int changeOfPacmanNumberY = 0;
	int changeOfPacmanNumberX = 0;

	if (isGoingRight) {
		changeOfPacmanNumberX += 4;
	}
	else if (isGoingLeft) {
		changeOfPacmanNumberX -= 4;
	}
	if (isGoingDown) {
		changeOfPacmanNumberY += 4;
	}
	else if (isGoingUp) {
		changeOfPacmanNumberY -= 4;
		changeOfPacmanNumberX -= 4;
	}

	int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };
	checkDirectionsAvailability(ghostCurX, ghostCurY, distance, changeOfPacmanNumberX, changeOfPacmanNumberY, pinkLastX, pinkLastY);
	ghostChangePosition(ghostCurX, ghostCurY, nextX, nextY, distance, pinkyNumber, pinkySymbol, pinkColor, pinkLastX, pinkLastY);
}

void shortestPathAlgorithmBlue() {
	int ghostCurX = getGhostXPosition(inkyNumber);
	int ghostCurY = getGhostYPosition(inkyNumber);
	int redCurX = getGhostXPosition(blinkyNumber);
	int redCurY = getGhostYPosition(blinkyNumber);
	int nextX = ghostCurX;
	int nextY = ghostCurY;
	int changeOfPacmanX = pacmanX;
	int changeOfPacmanY = pacmanY;

	if (isGoingUp) {
		changeOfPacmanY -= 2;
	}
	else if (isGoingDown) {
		changeOfPacmanY += 2;
	}
	else if (isGoingLeft) {
		changeOfPacmanX -= 2;
	}
	else if (isGoingRight) {
		changeOfPacmanX += 2;
	}

	int vectorX = changeOfPacmanX - redCurX;
	int vectorY = changeOfPacmanY - redCurY;

	int inkyTargetX = redCurX + 2 * vectorX;
	int inkyTargetY = redCurY + 2 * vectorY;

	int distance[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };

	checkDirectionsAvailability(ghostCurX, ghostCurY, distance, inkyTargetX, inkyTargetY, blueLastX, blueLastY);
	// Move Inky based on the smallest distance (best direction)
	ghostChangePosition(ghostCurX, ghostCurY, nextX, nextY, distance, inkyNumber, inkySymbol, blueColor, blueLastX, blueLastY);
}



void activateRedGhost() {
	shortestPathAlgorithmRed();
}
void activatePinkGhost() {
	shortestPathAlgorithmPink();
}
void activateBlueGhost() {
	shortestPathAlgorithmBlue();
}
void activateGreenGhost() {
	//shortestPathAlgorithm(clydeNumber);
	//>=8 distance same as Red but <8
}

void activateGhosts() {
	if (playerScore >= scoreToActivtRed) {
		activateRedGhost();
	}
	if (playerScore >= scoreToActivtPink) {
		activatePinkGhost();
	}

	if (playerScore >= scoreToActivtBlue) {
		activateBlueGhost();
	}
	/*
	if (playerScore >= scoreToActivtGreen) {
		activateGreenGhost();
	}*/
	isGoingDown = false;
	isGoingUp = false;
	isGoingRight = false;
	isGoingLeft = false;
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