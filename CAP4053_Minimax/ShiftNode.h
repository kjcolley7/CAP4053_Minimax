//
//  ShiftNode.h
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#ifndef MM_SHIFTNODE_H
#define MM_SHIFTNODE_H

#include "Board.h"

class PlaceNode;

// This represents a maximizing node
class ShiftNode {
public:
	ShiftNode(Board initBoard);
	
	void setBoard(Board newBoard);
	PlaceNode* getChild(Direction dir);
	void prune(ShiftNode* newHead);
	int getMaxScore(unsigned depth);
	int getMaxScore(unsigned depth, Direction* dir);
	
private:
	void populateChildren();
	
	PlaceNode* mUpChild;
	PlaceNode* mDownChild;
	PlaceNode* mLeftChild;
	PlaceNode* mRightChild;
	Board mBoard;
};

#endif /* MM_SHIFTNODE_H */
