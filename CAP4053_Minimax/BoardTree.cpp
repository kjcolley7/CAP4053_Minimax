//
//  BoardTree.cpp
//  CAP4053_Minimax
//
//  Created by Charlos Staszeski on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "BoardTree.h"
#include <climits>
#include <stack>

void BoardTree::populateTree() {
	unsigned depth = 0;
	std::stack<Node*> stack;
	stack.push(mHead);
	while(!stack.empty()) {
		Node* currNode = stack.top();
		stack.pop();
		if(mIsComputerTurn) {
			PlaceNode* place = (PlaceNode*)currNode;
			mIsComputerTurn = false;
			ShiftNode* children[4];
			place->getChildren(children);
			for(ShiftNode* node : children) {
				if(node != nullptr) {
					node->populateChildren();
					stack.push(node);
					int nodeScore = node->getScore();
					if(nodeScore < place->getScore())
						place->setScore(nodeScore);
				}
			}
		}
		else {
			ShiftNode* shift = (ShiftNode*)currNode;
			mIsComputerTurn = true;
			PlaceNode* children[16][2];
			shift->getChildren(children);
			for(int i = 0; i < 16; i++) {
				for(int j = 0; j < 2; j++) {
					PlaceNode* node = children[i][j];
					if(node != nullptr) {
						node->populateChildren();
						stack.push(children[i][j]);
						int nodeScore = node->getScore();
						if(nodeScore > shift->getScore())
							shift->setScore(nodeScore);
					}
				}
			}
		}
		depth++;
	}
}
BoardTree::BoardTree(Board initBoard) :
mHead(new PlaceNode(initBoard)) {
	mHead->populateChildren();
}

PlaceNode* BoardTree::getHead() {
	return mHead;
}

ShiftNode::ShiftNode(Board initBoard) : mBoard(initBoard)
{}

int ShiftNode::getScore() {
	return mScore;
}

void ShiftNode::setScore(int value) {
	mScore = value;
}

void ShiftNode::populateChildren() {
	int count = 0;
	int nullCount = 0;
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			for(int k = 0; k < 2; k++) {
				Board tempBoard = mBoard;
				int oldHoles[16];
				int newHoles[16];
				mBoard.findHoles(oldHoles);
				tempBoard.placeTile(k ? TILE_2 : TILE_4, i, j);
				tempBoard.findHoles(newHoles);
				bool holesHaveBeenChanged = false;
				for(int i = 0; i < 16; i++)
				{
					if(oldHoles[i] != newHoles[i]){
						holesHaveBeenChanged = true;
						break;
					}
				}
				if(holesHaveBeenChanged) {
					mChildren[count][k] = new PlaceNode(tempBoard);
					mChildren[count][k]->setScore(INT_MAX);
				}
				else
					nullCount++;
			}
			count++;
		}
	}
	if(nullCount == 32)
		mScore = mBoard.estimateScore();
}

void ShiftNode::getChildren(PlaceNode* children[16][2]) {
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 16; j++)
			children[j][i] = mChildren[j][i];
}

PlaceNode* ShiftNode::getChild(unsigned position, bool isTwo) {
	return mChildren[position][isTwo];
}

PlaceNode::PlaceNode(Board initBoard) : mBoard(initBoard)
{}

int PlaceNode::getScore() {
	return mScore;
}

void PlaceNode::setScore(int value) {
	mScore = value;
}

void PlaceNode::populateChildren() {
	Board tempBoard = mBoard;
	int nullCount = 0;
	
	if(tempBoard.shiftTilesUp()) {
		mUpChild = new ShiftNode(tempBoard);
		mUpChild->setScore(INT_MIN);
	}
	else
		nullCount++;

	tempBoard = mBoard;
	if(tempBoard.shiftTilesDown()) {
		mDownChild = new ShiftNode(tempBoard);
		mDownChild->setScore(INT_MIN);
	}
	else
		nullCount++;

	tempBoard = mBoard;
	if(tempBoard.shiftTilesLeft()) {
		mLeftChild = new ShiftNode(tempBoard);
		mLeftChild->setScore(INT_MIN);
	}
	else
		nullCount++;

	tempBoard = mBoard;
	if(tempBoard.shiftTilesRight()) {
		mRightChild = new ShiftNode(tempBoard);
		mRightChild->setScore(INT_MIN);
	}
	else
		nullCount++;

	if(nullCount == 4)
		mScore = mBoard.estimateScore();
}

void PlaceNode::getChildren(ShiftNode* children[4]) {
	children[0] = mUpChild;
	children[1] = mDownChild;
	children[2] = mLeftChild;
	children[3] = mRightChild;
}

ShiftNode* PlaceNode::getChild(Direction dir) {
	switch (dir) {
		case Direction::UP:
			return mUpChild;
			break;
		case Direction::DOWN:
			return mDownChild;
			break;
		case Direction::LEFT:
			return mLeftChild;
			break;
		case Direction::RIGHT:
			return mRightChild;
			break;
		default:
			break;
	}
}
