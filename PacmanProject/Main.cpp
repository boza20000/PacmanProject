
#include <iostream>
#include "Game.h"

// The main entry point of the game
int main() {
	Game& game = Game::game();  // Get the singleton instance of the Game class
	game.startPacmanGame();  // Start the Pacman game
	return 0;  // Exit the program
}