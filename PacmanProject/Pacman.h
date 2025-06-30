#pragma once

class Pacman {

public:
	// Function to check if Pacman has been caught by a ghost during chase mode
	static bool isPacmanCaughtByGhost();

	// Function to spawn Pacman at a random empty position on the grid
	static void spawnPacman();

	// Function to erase Pacman from its old position
	static void erasePacmanFromOldPosition();

	// Function to update Pacman's position on the grid
	static void updatePacmanGridPosition();

	// Function to clear Pacman's previous cell in the grid
	static void clearPacmanPreviousCell();
	// Function to set Pacman's current position in the grid
	static void setPacmanPosition();

	// Function to move Pacman in a specified direction
	static void movePacmanInDirection(int movingX, int movingY, char nextSymbol);

	// Function to move Pacman up if there is no wall
	static void movePacmanUp();

	// Function to move Pacman down if there is no wall
	static void movePacmanDown();

	// Function to move Pacman left if there is no wall
	static void movePacmanLeft();

	// Function to move Pacman right if there is no wall
	static void movePacmanRight();

	// Function to handle Pacman's movement based on key press (WASD keys)
	static void handlePacmanMovement();

	static int distanceToPacman(int ghostCurX, int ghostCurY, int changeX, int changeY);

	static void pacmanMovmentDirectionReset();

	//void spawnPacman() ;
	//void erasePacmanFromOldPosition() ;
	//void updatePacmanGridPosition() ;
	//void clearPacmanPreviousCell();
	//void setPacmanPosition() ;
	//void movePacmanInDirection(int movingX, int movingY, char nextSymbol);
	//void movePacmanUp();
	//void movePacmanDown();
	//void movePacmanLeft() ;
	//void movePacmanRight();
	//void handlePacmanMovement();
	//void updateScore(int points);
};
