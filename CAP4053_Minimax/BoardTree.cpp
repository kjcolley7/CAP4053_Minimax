//
//  BoardTree.cpp
//  CAP4053_Minimax
//
//  Created by Charlos Staszeski on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "BoardTree.h"

Board Node::getBoard() {
	return mBoard;
}
void BoardTree::populateTree() {
	unsigned depth = 0;
	Node* currNode = mHead;
	while(depth < kMaximumDepth) {
		if(mIsComputerTurn) {
			PlaceNode* shift = (PlaceNode*)currNode;
			mIsComputerTurn = false;
			ShiftNode* children[4];
			shift->getChildren(children);
			for(ShiftNode* node : children) {
				node->populateChildren();
			}
		}
		else {
			mIsComputerTurn = true;
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
