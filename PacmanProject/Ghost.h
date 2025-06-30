#pragma once

struct Position {
	int x;
	int y;
};

class Ghost
{
private:
	const char* color;
	Position position;
	const char symbol;
	const size_t scoreToActivate;
	const size_t ghostNumber;

public:
	Ghost(const char* color, Position position, char symbol, size_t scoreToActivate, size_t ghostNumber)
		: color(color), position(position), symbol(symbol), scoreToActivate(scoreToActivate), ghostNumber(ghostNumber) {
	}
	const char* getColor() const { return color; }
	Position getPosition() const { return position; }
	char getSymbol() const { return symbol; }
	size_t getScoreToActivate() const { return scoreToActivate; }
	size_t getGhostNumber() const { return ghostNumber; }
	void setPosition(int x, int y) {
		position.x = x;
		position.y = y;
	}
	virtual void activateGhost()const = 0;
	virtual void shortestPathAlgorithm()const = 0;
	virtual void moveBackwards(const char* color)const = 0;

	// Function to get the Y position of a specific ghost
	static int getGhostYPosition(int ghostNumber);
	// Function to get the X position of a specific ghost
	static int getGhostXPosition(int ghostNumber);
	// Function to set the position of a specific ghost
	static void setGhostPosition(int x, int y, int ghostNumber);

	// Function to repaint a ghost after moving it, changing its color
	static void repaintGhost(int ghostNumber, const char* color);

	// Function to spawn all ghosts at specific initial positions
	static void spawnGhost();

	// Function to erase a ghost from its old position when it moves
	static void eraseGhostFromOldPosition(int ghostNumber);

	// Function to move a ghost to a new position and repaint it with its color
	static void moveGhostTo(int x, int y, int ghostNumber, char symbol, const char* color);

	// Function to check if a given position collides with any ghost
	static bool isCollidedWithGhost(int x, int y);

	// Function to check if moving up is clear for a ghost (no wall or collision)
	static bool isDirectionUpClear(int ghostCurX, int ghostCurY, int lastX, int lastY);

	// Function to check if moving left is clear for a ghost (no wall or collision)
	static bool isDirectionLeftClear(int ghostCurX, int ghostCurY, int lastX, int lastY);

	// Function to check if moving right is clear for a ghost (no wall or collision)
	static bool isDirectionRightClear(int ghostCurX, int ghostCurY, int lastX, int lastY);

	// Function to check if moving down is clear for a ghost (no wall or collision)
	static bool isDirectionDownClear(int ghostCurX, int ghostCurY, int lastX, int lastY);

	// Function to check if moving backwards is possible for a ghost based on its current position
	static bool isBackwardsPossible(int x, int y);

	// Function to make all ghosts move backwards if their respective score conditions are met
	static void makeAllBackwardsMove();

	// Function to move a specific ghost backwards based on its current number (e.g., Blinky, Pinky, etc.)
	static void moveGhostBack(int curGhostNumber, const char* color);

	// Function to change the position of a ghost based on the shortest path, or if it can't move, move it backwards or stay
	static void ghostChangePosition(int ghostCurX, int ghostCurY, int distance[4], int ghostNumber, int ghostSymbol, const char* color, int& lastX, int& lastY);

	// Function to check the availability of directions (Up, Left, Down, Right) for the ghost to move
	static void checkDirectionsAvailability(int ghostCurX, int ghostCurY, int distance[], int changeX, int changeY, int lastX, int lastY);

	// Function to check if a ghost has been eaten by Pac-Man by checking their positions
	static int isGhostEatenByPacman();

	// Function to check if a corner is available for a ghost to move into
	static bool isCornerAvailable(int x, int y);

	// Function to check if a corner is clear for a ghost to move into
	static bool isCornerClear(int x, int y);

	// Function to send a ghost to a corner on the grid
	static void sendGhostToCorner(int ghostNumber);

	// Function to activate ghosts based on the player's score
	static int activateReadyGhosts();

	static bool tryBackwardsMovement(int validDirectionsCount, int lastX, int lastY, int ghostNumber, char ghostSymbol, const char* color);

	static void tryRandomValidDirect(bool directionValid[], int ghostCurX, int ghostCurY, int ghostNumber, char ghostSymbol, const char* color);

	// Function to move ghosts randomly within the grid based on available directions
	static void randomGhostMoves(int ghostCurX, int ghostCurY, int& lastX, int& lastY, int ghostNumber, char ghostSymbol, const char* color);

	static void frightenedModeActivated(int ghostCurX, int ghostCurY, int& lastX, int& lastY, int ghostNumber, char ghostSymbol, const char* color);

	//iterates through the ghosts that are awake and turns on the their frightened Mode
	static void frightenedModeGhosts();

	// Activate ghosts based on the player's score
	static void activateGhosts();

};


