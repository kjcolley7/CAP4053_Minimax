//
//  main.cpp
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 11/19/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#include <iostream>
#include <cctype>
#include <ctime>
#include <cstdlib>

#include "GameEngine.h"
#include "Board.h"

int main() {
//	GameEngine game{"2048 AI"};
//	return game.run();
	
	// Seed random number generator
	srand((unsigned)time(nullptr));
	
	// Create the board and place first two random tiles
	Board board;
	board.placeRandom();
	board.placeRandom();
	
	// Main game loop
	while(!board.isGameOver()) {
		// Display board
		board.print();
		
		// Get input choice
		char choice;
		std::cout << "Direction [ULDR] > ";
		std::cin >> choice;
		switch(::toupper(choice)) {
			case 'U':
				board.shiftTilesUp();
				break;
			
			case 'L':
				board.shiftTilesLeft();
				break;
			
			case 'D':
				board.shiftTilesDown();
				break;
			
			case 'R':
				board.shiftTilesRight();
				break;
			
			default:
				std::cout << "Invalid input! Enter U, L, D, or R." << std::endl;
				continue;
		}
		
		// Place next random value
		board.placeRandom();
	}
	
	// Loser
	std::cout << "Game over!" << std::endl;
	board.print();
	
	return 0;
}
