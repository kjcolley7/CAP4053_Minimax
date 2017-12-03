//
//  BoardTree.h
//  CAP4053_Minimax
//
//  Created by Charlos Staszeski on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#ifndef BoardTree_h
#define BoardTree_h

#include <stdio.h>
#include <memory>
#include "Board.h"
#include "Direction.h"

class PlaceNode;
class ShiftNode;

class BoardTree {
public:
	BoardTree(Board initBoard);
	void populateTree();
	PlaceNode* getHead();

private:
	PlaceNode* mHead;
	unsigned kMaximumDepth = 5;
	bool mIsComputerTurn;
};

// Generic node class to allow easier traversal through the tree
class Node {
public:
private:
	Board mBoard;
};

// This represents a minimizing node
class ShiftNode : public Node {
public:
	ShiftNode(Board initBoard);

	void getChildren(PlaceNode* children[16][2]);
	PlaceNode* getChild(unsigned position, bool isTwo);

	int getScore();
	void setScore(int value);

	void populateChildren();

private:
	PlaceNode* mChildren[16][2];
	Board mBoard;
	int mScore;
	PlaceNode* mMinNode;
};

// This represents a maximizing node
class PlaceNode : public Node {
public:
	PlaceNode(Board initBoard);

	void getChildren(ShiftNode* children[4]);
	ShiftNode* getChild(Direction dir);

	int getScore();
	void setScore(int value);

	void populateChildren();

private:
	ShiftNode* mUpChild;
	ShiftNode* mDownChild;
	ShiftNode* mLeftChild;
	ShiftNode* mRightChild;
	Board mBoard;
	int mScore;
	ShiftNode* mMaxNode;

	void populatePlaceNodes();
};

#endif /* BoardTree_h */
