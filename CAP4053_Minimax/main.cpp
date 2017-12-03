//
//  main.cpp
//  CAP4053_Minimax
//
//  Created by kTeam on 11/19/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include <iostream>
#include <cctype>
#include <ctime>
#include <cstdlib>

#include "Engine/GameEngine.h"
#include "Board.h"


int main() {
	// Seed random number generator
	srand((unsigned)time(nullptr));
	
	// Prepare the lookup tables
	Board::fillShiftTable();
	Board::fillScoreTable();
	
	// Run the game in a 600x800 portrait window
	GameEngine game{"2048 AI", 600, 800};
	return game.run();
}
