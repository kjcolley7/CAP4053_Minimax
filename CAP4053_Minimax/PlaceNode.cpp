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


PlaceNode::PlaceNode(Board initBoard)
: mBoard(initBoard) {
	memset(mChildren, 0, sizeof(mChildren));
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
					delete mChildren[i][j];
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
			
			mChildren[i][j] = new ShiftNode(*child++);
		}
	}
}


int PlaceNode::getMinScore(unsigned depth) {
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
				score = mChildren[i][j]->getMaxScore(depth);
				if(score < minScore) {
					minScore = score;
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
