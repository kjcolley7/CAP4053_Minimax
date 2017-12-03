//
//  ComputerPlayer.hpp
//  CAP4053_Minimax
//
//  Created by Charlos Staszeski on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "Board.h"
#include "Direction.h"
#include "BoardTree.h"

typedef std::pair<std::pair<unsigned, unsigned>, bool> newTilePlacement;

class ComputerPlayer {
public:
	ComputerPlayer();

	/**
	 * Runs the minimax algorthm to determine the move that minimizes the maximum loss
	 * @param currBoard The current state of the board
	 * @return The key press that leads to the best board state
	 */
	void populateBoardTree(Board currBoard);

private:
	bool mIsComputerTurn = true;
	BoardTree tree;
	static const unsigned kMaximumDepth = 5;
	Board minimax(Board currBoard);

	/**
	 * Given the current state of the board, calculates the optimal move for the player
	 * @param currBoard The state of the board at the time that the function was called
	 * @return The keypress for the most optimal move in the minimax tree
	 */
	Direction evaluateBestMove(Board currBoard);

	/**
	 * Given a board state, calculates the worst place to generate a tile
	 * @param currBoard The state of the board after tiles have shifted
	 * @return The worst position to place the tile (row, column) paired with a boolean
	 * indicating whether or not the tile is a 4
	 */
	newTilePlacement evaluateWorstMove(Board currBoard);
};
