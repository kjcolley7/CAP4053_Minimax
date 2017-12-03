//
//  PlaceNode.cpp
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "PlaceNode.h"
#include "ShiftNode.h"
#include <climits>
#include <cstring>


std::queue<PlaceNode*> PlaceNode::sPool;


PlaceNode* PlaceNode::allocate(Board initBoard) {
	if(!sPool.empty()) {
		PlaceNode* ret = sPool.front();
		sPool.pop();
		ret->init(initBoard);
		return ret;
	}
	
	return new PlaceNode(initBoard);
}


PlaceNode::PlaceNode(Board initBoard)
: mBoard(initBoard) {
	init(initBoard);
}


void PlaceNode::init(Board initBoard) {
	mBoard = initBoard;
	memset(mChildren, 0, sizeof(mChildren));
}


void PlaceNode::deallocate() {
	sPool.push(this);
}


ShiftNode* PlaceNode::getChild(unsigned row, unsigned col, Tile tile) {
	bool hasChild = false;
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 2; j++) {
			if(mChildren[i][j]) {
				hasChild = true;
			}
		}
	}
	
	if(!hasChild) {
		populateChildren();
	}
	
	return mChildren[row * 4 + col][tile-1];
}


void PlaceNode::prune(ShiftNode* newHead) {
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 2; j++) {
			if(mChildren[i][j]) {
				mChildren[i][j]->prune(newHead);
				if(mChildren[i][j] != newHead) {
					mChildren[i][j]->deallocate();
					mChildren[i][j] = nullptr;
				}
			}
		}
	}
}


void PlaceNode::populateChildren() {
	Board children[32];
	mBoard.allPlaces(children);
	
	Board* child = children;
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 2; j++) {
			if(child->isEmpty()) {
				++child;
				continue;
			}
			
			mChildren[i][j] = ShiftNode::allocate(*child++);
		}
	}
}


int PlaceNode::getMinScore(unsigned depth, int alpha, int beta) {
	if(depth == 0) {
		return mBoard.estimateScore();
	}
	
	int score, minScore = INT_MAX;
	bool hasChild = false;
	
hereIAmOnceAgain:
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 2; j++) {
			if(mChildren[i][j]) {
				hasChild = true;
				
				// Intentionally not decrementing depth here
				score = mChildren[i][j]->getMaxScore(depth, alpha, beta);
				if(score < minScore) {
					minScore = score;
				}
				if(minScore < beta) {
					beta = minScore;
				}
				if(alpha >= beta) {
					return minScore;
				}
			}
		}
	}
	
	// No children? Probably need to populate children and try again
	if(!hasChild) {
		hasChild = true;
		populateChildren();
		goto hereIAmOnceAgain;
	}
	
	return minScore;
}
