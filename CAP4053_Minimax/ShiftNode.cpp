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


ShiftNode::ShiftNode(Board initBoard)
: mBoard(initBoard), mUpChild(nullptr), mDownChild(nullptr), mLeftChild(nullptr), mRightChild(nullptr) { }



void ShiftNode::setBoard(Board newBoard) {
	prune(nullptr);
	mBoard = newBoard;
}


PlaceNode* ShiftNode::getChild(Direction dir) {
	if(!(mUpChild || mDownChild || mLeftChild || mRightChild)) {
		populateChildren();
	}
	
	switch(dir) {
		case Direction::UP: return mUpChild;
		case Direction::DOWN: return mDownChild;
		case Direction::LEFT: return mLeftChild;
		case Direction::RIGHT: return mRightChild;
	}
}


void ShiftNode::prune(ShiftNode* newHead) {
	if(this == newHead) {
		return;
	}
	
	if(mUpChild) {
		mUpChild->prune(newHead);
		delete mUpChild;
		mUpChild = nullptr;
	}
	
	if(mDownChild) {
		mDownChild->prune(newHead);
		delete mDownChild;
		mDownChild = nullptr;
	}
	
	if(mLeftChild) {
		mLeftChild->prune(newHead);
		delete mLeftChild;
		mLeftChild = nullptr;
	}
	
	if(mRightChild) {
		mRightChild->prune(newHead);
		delete mRightChild;
		mRightChild = nullptr;
	}
}


void ShiftNode::populateChildren() {
	Board up, down, left, right;
	mBoard.allShifts(&up, &down, &left, &right);
	
	if(!up.isEmpty()) {
		mUpChild = new PlaceNode(up);
	}
	
	if(!down.isEmpty()) {
		mDownChild = new PlaceNode(down);
	}
	
	if(!left.isEmpty()) {
		mLeftChild = new PlaceNode(left);
	}
	
	if(!right.isEmpty()) {
		mRightChild = new PlaceNode(right);
	}
}


// Smaller stack frame
int ShiftNode::getMaxScore(unsigned depth) {
	if(depth == 0) {
		return mBoard.estimateScore();
	}
	
	// Populate children if they haven't been yet
	if(!(mUpChild || mDownChild || mLeftChild || mRightChild)) {
		populateChildren();
	}
	
	int score, maxScore = INT_MIN;
	
	// Score children
	if(mUpChild) {
		score = mUpChild->getMinScore(depth - 1);
		if(score > maxScore) {
			maxScore = score;
		}
	}
	
	if(mDownChild) {
		score = mDownChild->getMinScore(depth - 1);
		if(score > maxScore) {
			maxScore = score;
		}
	}
	
	if(mLeftChild) {
		score = mLeftChild->getMinScore(depth - 1);
		if(score > maxScore) {
			maxScore = score;
		}
	}
	
	if(mRightChild) {
		score = mRightChild->getMinScore(depth - 1);
		if(score > maxScore) {
			maxScore = score;
		}
	}
	
	return maxScore;
}


int ShiftNode::getMaxScore(unsigned depth, Direction* dir) {
	if(depth == 0) {
		return mBoard.estimateScore();
	}
	
	// Populate children if they haven't been yet
	if(!(mUpChild || mDownChild || mLeftChild || mRightChild)) {
		populateChildren();
	}
	
	int score, maxScore = INT_MIN;
	Direction maxDir;
	
	// Score children
	if(mUpChild) {
		score = mUpChild->getMinScore(depth - 1);
		if(score > maxScore) {
			maxScore = score;
			maxDir = Direction::UP;
		}
	}
	
	if(mDownChild) {
		score = mDownChild->getMinScore(depth - 1);
		if(score > maxScore) {
			maxScore = score;
			maxDir = Direction::DOWN;
		}
	}
	
	if(mLeftChild) {
		score = mLeftChild->getMinScore(depth - 1);
		if(score > maxScore) {
			maxScore = score;
			maxDir = Direction::LEFT;
		}
	}
	
	if(mRightChild) {
		score = mRightChild->getMinScore(depth - 1);
		if(score > maxScore) {
			maxScore = score;
			maxDir = Direction::RIGHT;
		}
	}
	
	if(maxScore == INT_MIN) {
		if(mUpChild) {
			maxDir = Direction::UP;
		}
		else if(mDownChild) {
			maxDir = Direction::DOWN;
		}
		else if(mLeftChild) {
			maxDir = Direction::LEFT;
		}
		else if(mRightChild) {
			maxDir = Direction::RIGHT;
		}
	}
	
	if(dir) {
		*dir = maxDir;
	}
	return maxScore;
}
