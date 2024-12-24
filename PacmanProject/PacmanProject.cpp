#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h> 
#include <fstream>

int pacmanX, pacmanY;
int playerScore = 0;

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
const char* greenColor ="\033[32m";
const char* endColor = "\033[0m";

const char scoreToActivtRed = 0;
const char scoreToActivtPink = 20;
const char scoreToActivtBlue = 40;
const char scoreToActivtGreen = 60;

const size_t foodAmount = 6;
const size_t prizeOfFood = 20;
const size_t prizeOfPoint = 1;

int foodX[foodAmount], foodY[foodAmount];
bool isGhostRemovable = false;
bool isNormalMode = true;
bool isChaseMode = false;
const size_t wallsIncl = 2;

char** grid = nullptr;
int widthGrid = 0, heightGrid = 0;
std::ifstream file("map.txt");


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

void allocateMemoryForGrid(int rows, int cols) {
    grid = new char* [rows];
    for (int i = 0; i < rows ; i++) {
        grid[i] = new char[cols];
        for (int j = 0; j < cols ; j++) {
            grid[i][j] = ' ';
        }
    }
}

void deallocateMemoryForGrid() {
    if (grid != nullptr) {
        for (int i = 0; i < heightGrid ; i++) {
            delete[] grid[i];
        }
        delete[] grid;
        grid = nullptr;
    }
}
void ignoreLineTillEnd() {
    file.ignore(std::streamsize(INT_MAX), '\n'); // Skip to the next line
}

//put walls over the sides that are cut 
void putEndWalls() {

    for (size_t i = 0; i < heightGrid ; i++)
    {
        for (size_t j = 0; j < widthGrid ; j++)
        {
            if ((heightGrid-1) == i || (widthGrid-1) == j) {
                grid[i][j] = '#';
            }
        }
    }

}

//gest the map from a file with the height and width that the user inputed
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

    putEndWalls();
}

//displayes the map 
void displayMap() {
    for (int i = 0; i < heightGrid ; ++i) {
        for (int j = 0; j < widthGrid; ++j) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

//starts the game and waits for grid paremeters
void startGame() {
    std::cout << "Enter grid width: ";
    std::cin >> widthGrid;
    std::cout << "Enter grid height: ";
    std::cin >> heightGrid;

    if (widthGrid <= 0 || heightGrid <= 0) {
        std::cerr << "Error: Grid dimensions must be positive integers under 50(incl.)" << std::endl;
        std::exit(1);
    }
    if (widthGrid<49 || heightGrid<49) {
        widthGrid += wallsIncl;
        heightGrid += wallsIncl;
    }
    
    allocateMemoryForGrid(heightGrid , widthGrid );
    clearConsole();
}

//waits for keystrock to end the game
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

//visualises the player score
void displayPlayerScore() {
    setCursorPosition(widthGrid * 2 + 1, heightGrid / 2);
    std::cout << "Score: " << playerScore << "  ";
}

//updates score of the player
void updateScore(int points) {
    playerScore += points;  
    displayPlayerScore();   
}

//game over screen
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

//check whether all food is collected
bool collectedAllFood() {
    for (size_t i = 0; i < foodAmount; i++) {
        if (grid[foodY[i]][foodX[i]] == foodSymbol) {
            return false;
        }
    }
    return true;
}

//check for collision with ghost
bool CollidesWithGhost() {
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
bool gameOver() {
    return CollidesWithGhost() || collectedAllFood(); 
}

void startFrightenMode() {
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
void clearOldPositionPacman() {
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
        clearOldPositionPacman();
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
        clearOldPositionPacman();
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
        clearOldPositionPacman();
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
        clearOldPositionPacman();
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

int getGhostYCoord(int ghostNumber) {
    return ghostY[ghostNumber];
}

int getGhostXCoord(int ghostNumber) {
    return ghostX[ghostNumber];
}

void setGhostCoord(int x, int y, int ghostNumber) {
    ghostX[ghostNumber] = x;
    ghostY[ghostNumber] = y;
}

void repaintGhost(int ghostNumber, const char* color) {
    int x = getGhostXCoord(ghostNumber);
    int y = getGhostYCoord(ghostNumber);

    setCursorPosition(x * 2, y);
    std::cout << emptySymbol;

    setCursorPosition(x * 2, y);
    std::cout << color << grid[y][x] << endColor;
}

void spawnGhost() {
    
    //starting coords
    //put in array for ghosts
    setGhostCoord(1, 2, blinkyNumber);  // Blinky
    setGhostCoord(1, 1, pinkyNumber);  // Pinky
    setGhostCoord(2, 1, inkyNumber);  // Inky
    setGhostCoord(3, 1, clydeNumber);  // Clyde

    
    grid[ghostY[0]][ghostX[0]] = blinkySymbol;
    grid[ghostY[1]][ghostX[1]] = pinkySymbol;
    grid[ghostY[2]][ghostX[2]] = inkySymbol;
    grid[ghostY[3]][ghostX[3]] = clydeSymbol;

    repaintGhost(blinkyNumber, redColor);  // Red for Blinky
    repaintGhost(pinkyNumber, pinkColor);  // Pink for Pinky
    repaintGhost(inkyNumber, blueColor);  // Blue for Inky
    repaintGhost(clydeNumber, greenColor);  // Green for Clyde
}

void ghostActivation() {
    if (playerScore>= scoreToActivtRed) {
        //activateRed
    }
    if (playerScore>= scoreToActivtPink) {
        //activatePink
    }
    if (playerScore>= scoreToActivtBlue) {
        //activateBLue
    }
    if (playerScore >= scoreToActivtGreen) {
        //activateGreen
    }
}


void InitializeGame() {
    hideCursor();
    srand(time(0));
    startGame();
    loadFileMap();
    displayMap();
    spawnGhost();
    spawnPacman();
    spawnFood();   
}

void GameLoop() {
    while (!gameOver()) {
        ghostActivation();
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