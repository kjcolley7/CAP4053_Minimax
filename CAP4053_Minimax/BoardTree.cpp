//
//  BoardTree.cpp
//  CAP4053_Minimax
//
//  Created by Charlos Staszeski on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "BoardTree.h"
#include <iostream>


const unsigned BoardTree::kMaximumDepth = 5;


BoardTree::BoardTree(Board initBoard)
: mHead(ShiftNode::allocate(initBoard)), mBestMove(Direction::UP) { }



void BoardTree::setBoard(Board newBoard) {
	mHead->setBoard(newBoard);
}


bool BoardTree::isValid() const {
	return mHead != nullptr;
}


Direction BoardTree::getBestMove() {
	// If there are few holes left on the board, allow going one level deeper
	int depth = kMaximumDepth;
	{
		Board board = mHead->getBoard();
		int holes[16];
		unsigned holeCount = board.findHoles(holes);
		if(holeCount >= 3) {
			--depth;
		}
		if(holeCount >= 12) {
			--depth;
		}
	}
	
	// Compute max score
	int score = mHead->getMaxScore(depth, INT_MIN, INT_MAX, &mBestMove);
	
	// Get printable direction for log
	char cDir;
	switch(mBestMove) {
		case Direction::UP:
			cDir = 'U';
			break;
		
		case Direction::DOWN:
			cDir = 'D';
			break;
		
		case Direction::LEFT:
			cDir = 'L';
			break;
		
		case Direction::RIGHT:
			cDir = 'R';
			break;
	}
	
	// Log results
	std::cerr << "Picking direction " << cDir << " with score " << score << std::endl;
	return mBestMove;
}


void BoardTree::placedTile(unsigned row, unsigned col, Tile tile) {
	PlaceNode* firstMove = mHead->getChild(mBestMove);
	if(firstMove) {
		updateHead(firstMove->getChild(row, col, tile));
	}
	else {
		updateHead(nullptr);
	}
}


void BoardTree::updateHead(ShiftNode* newHead) {
	mHead->prune(newHead);
	mHead->deallocate();
	mHead = newHead;
}
