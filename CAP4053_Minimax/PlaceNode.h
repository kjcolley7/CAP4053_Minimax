//
//  PlaceNode.h
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#ifndef MM_PLACENODE_H
#define MM_PLACENODE_H

#include <queue>
#include <climits>
#include "Board.h"

class ShiftNode;

// This represents a minimizing node
class PlaceNode {
public:
	static PlaceNode* allocate(Board initBoard);
	
	PlaceNode(Board initBoard);
	void init(Board initBoard);
	void deallocate();
	
	ShiftNode* getChild(unsigned row, unsigned col, Tile tile);
	void prune(ShiftNode* newHead);
	int getMinScore(unsigned depth, int alpha = INT_MIN, int beta = INT_MAX);
	
private:
	void populateChildren();
	
	static std::queue<PlaceNode*> sPool;
	
	ShiftNode* mChildren[16][2];
	Board mBoard;
};

#endif /* MM_PLACENODE_H */
