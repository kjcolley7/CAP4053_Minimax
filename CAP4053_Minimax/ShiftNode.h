//
//  ShiftNode.h
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#ifndef MM_SHIFTNODE_H
#define MM_SHIFTNODE_H

#include <queue>
#include "Board.h"

class PlaceNode;

// This represents a maximizing node
class ShiftNode {
public:
	static ShiftNode* allocate(Board initBoard);
	
	ShiftNode(Board initBoard);
	void init(Board initBoard);
	void deallocate();
	
	Board getBoard() const;
	void setBoard(Board newBoard);
	PlaceNode* getChild(Direction dir);
	void prune(ShiftNode* newHead);
	int getMaxScore(unsigned depth, int alpha, int beta);
	int getMaxScore(unsigned depth, int alpha, int beta, Direction* dir);
	
private:
	void populateChildren();
	
	static const PlaceNode* kEmptyChildren[4];
	
	static std::queue<ShiftNode*> sPool;
	
	PlaceNode* mChildren[4];
	Board mBoard;
};

#endif /* MM_SHIFTNODE_H */
