#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h> 
#include <fstream>

int pacmanX, pacmanY;
const size_t amountOfGhosts = 4;
int ghostX[amountOfGhosts], ghostY[amountOfGhosts];
int playerScore = 0;
const char pacmanSymbol = 'P';
const char wallSymbol = '#';
const char foodSymbol = '@';
const char pointSymbol = '.';
const char emptySymbol = ' ';
const int foodAmount = 4;
const int prizeOfFood = 25;
const int prizeOfPoint = 0;
int foodX[foodAmount], foodY[foodAmount];
bool isRemovable = false;

char** grid = nullptr;
int widthGrid = 0, heightGrid = 0;
std::ifstream file("map.txt");

void allocateMemoryForGrid(int rows, int cols) {
    grid = new char* [rows];
    for (int i = 0; i < rows; ++i) {
        grid[i] = new char[cols];
        for (int j = 0; j < cols; ++j) {
            grid[i][j] = ' ';
        }
    }
}

void deallocateMemoryForGrid() {
    if (grid != nullptr) {
        for (int i = 0; i < heightGrid; i++) {
            delete[] grid[i];
        }
        delete[] grid;
        grid = nullptr;
    }
}
void ignoreLineTillEnd() {
    file.ignore(std::streamsize(INT_MAX), '\n'); // Skip to the next line
}

void loadFileMap() {
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
        ignoreLineTillEnd();
    }
    file.close();
}


void displayMap() {
    for (int i = 0; i < heightGrid; ++i) {
        for (int j = 0; j < widthGrid; ++j) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void start() {
    std::cout << "Enter grid width: ";
    std::cin >> widthGrid;
    std::cout << "Enter grid height: ";
    std::cin >> heightGrid;

    if (widthGrid <= 0 || heightGrid <= 0) {
        std::cerr << "Error: Grid dimensions must be positive integers under 50(incl.)" << std::endl;
        std::exit(1);
    }

    allocateMemoryForGrid(heightGrid, widthGrid);
}

void setCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hConsole, position);
}
void hideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
void clearConsole() {
    system("cls");
}
void waitForEnd() {
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
void displayPlayerScore() {
    setCursorPosition(widthGrid * 2, heightGrid / 2);
    std::cout << "Score: " << playerScore << "  ";
}

//test
// Function to update the score (just for testing purposes)
void updateScore(int points) {
    playerScore += points;  // Add points to the player score
    displayPlayerScore();   // Display updated score
}

void displayGameOver() {
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
    waitForEnd();
}

//the game is not over untill the player collects the foods
bool gameOver() {
    for (size_t i = 0; i < foodAmount; i++) {
        if (grid[foodY[i]][foodX[i]] == foodSymbol) {
            return false;
        }
    }
    return true;
}

void startFrightenMode() {
    isRemovable = true;
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
        setCursorPosition(foodXCur * 2, foodYCur);
        std::cout << foodSymbol;
    }
}

bool isFoodTaken(char ch) {
    return ch == foodSymbol;
}
bool isPointTaken(char ch) {
    return ch == pointSymbol;
}

void setScreenSize() {
    //create a function that handels big matrix (consol handles to 30x30)
}

//void drawGrid() {
//    for (size_t i = 0; i < gridHeight; i++) {
//        for (size_t j = 0; j < gridWidth; j++) {
//           
//            if (i == 0 || j == 0 || i == (gridHeight - 1) || j == (gridWidth - 1)) {
//                grid[i][j] = wallSymbol;
//            }
//            else {
//                grid[i][j] = pointSymbol;
//            }
//            std::cout << grid[i][j] << " "; 
//        }
//        std::cout << std::endl; 
//    }
//}

void spawnPacman() {
    do {
        pacmanX = rand() % (widthGrid - 2) + 1;
        pacmanY = rand() % (heightGrid - 2) + 1;
    } while (grid[pacmanY][pacmanX] == wallSymbol);

    grid[pacmanY][pacmanX] = pacmanSymbol;
    setCursorPosition(pacmanX * 2, pacmanY);  // Adjust cursor for spacing
    std::cout << pacmanSymbol;
}

void updateCell(int x, int y, char symbol) {
    setCursorPosition(x * 2, y);  // Adjust cursor for grid spacing
    std::cout << symbol;  // Update only the specified cell
}
void clearOldPosition() {
    updateCell(pacmanX, pacmanY, emptySymbol);
}

void updatePacmanGridPosition() {
    updateCell(pacmanX, pacmanY, pacmanSymbol);
}

void setOldPlayerPositionEmpty() {
    grid[pacmanY][pacmanX] = emptySymbol;
}

void setPacmanPosition() {
    grid[pacmanY][pacmanX] = pacmanSymbol;
}


void moveUp() {
    if (grid[pacmanY - 1][pacmanX] != wallSymbol) {
        char nextSymbol = grid[pacmanY - 1][pacmanX];
        if (isFoodTaken(nextSymbol)) {
            updateScore(prizeOfFood);
        }
        else if (isPointTaken(nextSymbol)) {
            updateScore(prizeOfPoint);
        }
        setOldPlayerPositionEmpty();
        clearOldPosition();
        pacmanY -= 1;
        setPacmanPosition();
        updatePacmanGridPosition();
    }
}

void moveDown() {
    if (grid[pacmanY + 1][pacmanX] != wallSymbol) {
        char nextSymbol = grid[pacmanY + 1][pacmanX];
        if (isFoodTaken(nextSymbol)) {
            updateScore(prizeOfFood);
        }
        else if (isPointTaken(nextSymbol)) {
            updateScore(prizeOfPoint);
        }
        setOldPlayerPositionEmpty();
        clearOldPosition();
        pacmanY += 1;
        setPacmanPosition();
        updatePacmanGridPosition();

    }
}

void moveLeft() {
    if (grid[pacmanY][pacmanX - 1] != wallSymbol) {
        char nextSymbol = grid[pacmanY][pacmanX - 1];
        if (isFoodTaken(nextSymbol)) {
            updateScore(prizeOfFood);
        }
        else if (isPointTaken(nextSymbol)) {
            updateScore(prizeOfPoint);
        }
        setOldPlayerPositionEmpty();
        clearOldPosition();
        pacmanX -= 1;
        setPacmanPosition();
        updatePacmanGridPosition();
    }
}

void moveRight() {
    if (grid[pacmanY][pacmanX + 1] != wallSymbol) {
        char nextSymbol = grid[pacmanY][pacmanX + 1];
        if (isFoodTaken(nextSymbol)) {
            updateScore(prizeOfFood);
        }
        else if (isPointTaken(nextSymbol)) {
            updateScore(prizeOfPoint);
        }
        setOldPlayerPositionEmpty();
        clearOldPosition();
        pacmanX += 1;
        setPacmanPosition();
        updatePacmanGridPosition();
    }
}

void movePacman() {
    const int KEY_PRESSED_MASK = 0x8000;
    if (GetAsyncKeyState('W') & KEY_PRESSED_MASK) {  // W key pressed (up)
        moveUp();
    }
    if (GetAsyncKeyState('S') & KEY_PRESSED_MASK) {  // S key pressed (down)
        moveDown();
    }
    if (GetAsyncKeyState('A') & KEY_PRESSED_MASK) {  // A key pressed (left)
        moveLeft();
    }
    if (GetAsyncKeyState('D') & KEY_PRESSED_MASK) {  // D key pressed (right)
        moveRight();
    }
}

void InitializeGame() {
    hideCursor();
    srand(time(0));

    // spawnPacman();
    // spawnFood();
}

void GameLoop() {
    while (!gameOver()) {
        movePacman();
        displayPlayerScore();
        Sleep(120);
    }
    Sleep(1000);
    displayGameOver();
}

//the function calls the game
void runGamePacMan() {
    InitializeGame();
    GameLoop();
}

int main() {
    runGamePacMan();
    return 0;
}