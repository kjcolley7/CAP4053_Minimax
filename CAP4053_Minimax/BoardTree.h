//
//  BoardTree.h
//  CAP4053_Minimax
//
//  Created by Charlos Staszeski on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#ifndef MM_BOARDTREE_H
#define MM_BOARDTREE_H

#include <memory>
#include <cstdio>
#include "Board.h"
#include "Direction.h"
#include "ShiftNode.h"
#include "PlaceNode.h"

class BoardTree {
public:
	BoardTree(Board initBoard);
	
	void setBoard(Board newBoard);
	bool isValid() const;
	void populateTree();
	Direction getBestMove();
	void placedTile(unsigned row, unsigned col, Tile tile);

private:
	void updateHead(ShiftNode* newHead);
	
	static const unsigned kMaximumDepth;
	
	ShiftNode* mHead;
	Direction mBestMove;
};

#endif /* MM_BOARDTREE_H */
