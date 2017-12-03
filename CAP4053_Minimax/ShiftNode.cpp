//
//  ShiftNode.cpp
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "ShiftNode.h"
#include "PlaceNode.h"
#include <climits>
#include <cstring>


const PlaceNode* ShiftNode::kEmptyChildren[4] = {};

std::queue<ShiftNode*> ShiftNode::sPool;


ShiftNode* ShiftNode::allocate(Board initBoard) {
	if(!sPool.empty()) {
		ShiftNode* ret = sPool.front();
		sPool.pop();
		ret->init(initBoard);
		return ret;
	}
	
	return new ShiftNode(initBoard);
}

ShiftNode::ShiftNode(Board initBoard)
: mBoard(initBoard) {
	init(initBoard);
}


void ShiftNode::init(Board initBoard) {
	mBoard = initBoard;
	memset(mChildren, 0, sizeof(mChildren));
}


void ShiftNode::deallocate() {
	sPool.push(this);
}


void ShiftNode::setBoard(Board newBoard) {
	prune(nullptr);
	mBoard = newBoard;
}


PlaceNode* ShiftNode::getChild(Direction dir) {
	if(memcmp(mChildren, kEmptyChildren, sizeof(mChildren)) == 0) {
		populateChildren();
	}
	
	return mChildren[(int)dir];
}


void ShiftNode::prune(ShiftNode* newHead) {
	if(this == newHead) {
		return;
	}
	
	for(int i = 0; i < 4; i++) {
		if(mChildren[i]) {
			mChildren[i]->prune(newHead);
			mChildren[i]->deallocate();
			mChildren[i] = nullptr;
		}
	}
}


void ShiftNode::populateChildren() {
	Board shifts[4];
	mBoard.allShifts(shifts);
	
	for(int i = 0; i < 4; i++) {
		if(!shifts[i].isEmpty()) {
			mChildren[i] = PlaceNode::allocate(shifts[i]);
		}
	}
}


// Smaller stack frame
int ShiftNode::getMaxScore(unsigned depth, int alpha, int beta) {
	if(depth == 0) {
		return mBoard.estimateScore();
	}
	
	// Populate children if they haven't been yet
	if(memcmp(mChildren, kEmptyChildren, sizeof(mChildren)) == 0) {
		populateChildren();
	}
	
	int score, maxScore = INT_MIN;
	
	// Score children
	for(int i = 0; i < 4; i++) {
		if(mChildren[i]) {
			score = mChildren[i]->getMinScore(depth - 1, alpha, beta);
			if(score > maxScore) {
				maxScore = score;
			}
			if(maxScore > alpha) {
				alpha = maxScore;
			}
			if(alpha >= beta) {
				return maxScore;
			}
		}
	}
	
	return maxScore;
}


int ShiftNode::getMaxScore(unsigned depth, int alpha, int beta, Direction* dir) {
	if(depth == 0) {
		return mBoard.estimateScore();
	}
	
	// Populate children if they haven't been yet
	if(memcmp(mChildren, kEmptyChildren, sizeof(mChildren)) == 0) {
		populateChildren();
	}
	
	int score, maxScore = INT_MIN;
	Direction maxDir;
	
	// Score children
	for(int i = 0; i < 4; i++) {
		if(mChildren[i]) {
			score = mChildren[i]->getMinScore(depth - 1, alpha, beta);
			if(score > maxScore) {
				maxScore = score;
				maxDir = (Direction)i;
			}
			if(maxScore > alpha) {
				alpha = maxScore;
			}
			if(alpha >= beta) {
				return maxScore;
			}
		}
	}
	
	// All directions score as INT_MIN, so just pick any one which is nonnull
	if(maxScore == INT_MIN) {
		for(int i = 0; i < 4; i++) {
			if(mChildren[i]) {
				maxDir = (Direction)i;
				break;
			}
		}
	}
	
	if(dir) {
		*dir = maxDir;
	}
	return maxScore;
}
