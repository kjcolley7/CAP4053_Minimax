//
//  Board.cpp
//  CAP4053_Minimax
//
//  Created by kTeam on 11/19/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "Board.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "BoardPrivate.h"


static inline CompressedGrid applyingTile(CompressedGrid grid, int shift, CompressedGrid tile) {
	return ((grid & ~(TILE_MASK << shift)) | (tile << shift));
}

static inline CompressedGrid settingTile(CompressedGrid grid, unsigned row, unsigned col, int tile) {
	return applyingTile(grid, MAKE_SHIFT(row, col), tile);
}


/*
 * These shifting<Row/Column><Direction> functions all do pretty much the same thing. They use dst,
 * which is a shift value (grid location), to decide where the tiles should be shifted to. Then, delta
 * is a shift value that decides how far the tiles are shifting, whether it's by one, two, or three cells.
 * Take for example the following grid:
 *
 *   2 | 4 | 2 | 8
 *  ---+---+---+---
 *     | . |   |
 *  ---+---+---+---
 *     | 4 |   |
 *  ---+---+---+---
 *     | 4 |   |
 *
 * Calling shiftingColumnUp(grid, MAKE_SHIFT(1, 1)) would produce the following result:
 *
 *   2 | 4 | 2 | 8
 *  ---+---+---+---
 *     | 4 |   |
 *  ---+---+---+---
 *     | 4 |   |
 *  ---+---+---+---
 *     |   |   |
 *
 */
static inline CompressedGrid shiftingColumnUp(CompressedGrid grid, int dst, int delta = MAKE_ROW_SHIFT(1)) {
	for(int src = dst + delta;
		GET_SHIFT_ROW(dst) < GET_SHIFT_ROW(src);
		dst = MAKE_DOWN(dst), src = dst + delta
	) {
		grid = applyingTile(grid, dst, EXTRACT_TILE(grid, src));
	}
	for(; GET_SHIFT_ROW(dst) != 0; dst = MAKE_DOWN(dst)) {
		grid = applyingTile(grid, dst, TILE_EMPTY);
	}
	return grid;
}

static inline CompressedGrid shiftingColumnDown(CompressedGrid grid, int dst, int delta = MAKE_ROW_SHIFT(1)) {
	for(int src = dst - delta;
		GET_SHIFT_ROW(src) < GET_SHIFT_ROW(dst);
		dst = MAKE_UP(dst), src = dst - delta
	) {
		grid = applyingTile(grid, dst, EXTRACT_TILE(grid, src));
	}
	for(; GET_SHIFT_ROW(dst) != 3; dst = MAKE_UP(dst)) {
		grid = applyingTile(grid, dst, TILE_EMPTY);
	}
	return grid;
}

static inline CompressedGrid shiftingRowLeft(CompressedGrid grid, int dst, int delta = MAKE_COL_SHIFT(1)) {
	for(int src = dst + delta;
		GET_SHIFT_COL(dst) < GET_SHIFT_COL(src);
		dst = MAKE_RIGHT(dst), src = dst + delta
	) {
		grid = applyingTile(grid, dst, EXTRACT_TILE(grid, src));
	}
	for(; GET_SHIFT_COL(dst) != 0; dst = MAKE_RIGHT(dst)) {
		grid = applyingTile(grid, dst, TILE_EMPTY);
	}
	return grid;
}

static inline CompressedGrid shiftingRowRight(CompressedGrid grid, int dst, int delta = MAKE_COL_SHIFT(1)) {
	for(int src = dst - delta;
		GET_SHIFT_COL(src) < GET_SHIFT_COL(dst);
		dst = MAKE_LEFT(dst), src = dst - delta
	) {
		grid = applyingTile(grid, dst, EXTRACT_TILE(grid, src));
	}
	for(; GET_SHIFT_COL(dst) != 3; dst = MAKE_LEFT(dst)) {
		grid = applyingTile(grid, dst, TILE_EMPTY);
	}
	return grid;
}


static inline CompressedGrid shiftingTilesUp(CompressedGrid grid) {
	// Iterate through each tile from top to bottom and then left to right within each row
	for(int shift = MAKE_SHIFT(0, 0); GET_SHIFT_ROW(shift) != 3; shift = MAKE_RIGHT(shift)) {
		// First skip past empty spaces
		unsigned tile = EXTRACT_TILE(grid, shift);
		if(tile == TILE_EMPTY) {
			/*
			 *   X 0 0 X -> X A 0 X
			 *   X 0 0 X -> X A 0 X
			 *   X A 0 X -> X 0 0 X
			 *   X A 0 X -> X 0 0 X
			 */
			
			// Count how many zeros we're shifting past
			int delta_shift = MAKE_ROW_SHIFT(1);
			while(GET_SHIFT_ROW(shift + delta_shift) != 0
			      && EXTRACT_TILE(grid, shift + delta_shift) == TILE_EMPTY
			) {
				delta_shift += MAKE_ROW_SHIFT(1);
			}
			
			// Make sure it's not just an entire line of zeros
			if(GET_SHIFT_ROW(shift + delta_shift) != 0) {
				grid = shiftingColumnUp(grid, shift, delta_shift);
			}
		}
	}
	
	// Now try to merge tiles
	for(int shift = MAKE_SHIFT(0, 0); shift < MAKE_SHIFT(3, 0); shift = MAKE_RIGHT(shift)) {
		unsigned tile = EXTRACT_TILE(grid, shift);
		if(tile != TILE_EMPTY && tile == EXTRACT_TILE(grid, MAKE_DOWN(shift))) {
			/*
			 *   X A A X -> X B B X
			 *   X A A X -> X B B X
			 *   X B A X -> X C 0 X
			 *   X C A X -> X 0 0 X
			 */
			grid = applyingTile(grid, shift, tile + 1);
			grid = shiftingColumnUp(grid, MAKE_DOWN(shift));
		}
	}
	
	return grid;
}


static inline CompressedGrid shiftingTilesDown(CompressedGrid grid) {
	// Iterate through each tile from bottom to top and then right to left within each row
	for(int shift = MAKE_SHIFT(3, 3); shift >= MAKE_SHIFT(0, 0); shift = MAKE_LEFT(shift)) {
		// First skip past empty spaces
		unsigned tile = EXTRACT_TILE(grid, shift);
		if(tile == TILE_EMPTY) {
			/*
			 *   X A 0 X -> X 0 0 X
			 *   X B 0 X -> X A 0 X
			 *   X C 0 X -> X B 0 X
			 *   X 0 0 X -> X C 0 X
			 */
			
			// Count how many zeros we're shifting past
			int delta_shift = MAKE_ROW_SHIFT(1);
			while(GET_SHIFT_ROW(shift - delta_shift) != 3
			      && EXTRACT_TILE(grid, shift - delta_shift) == TILE_EMPTY
			) {
				delta_shift += MAKE_ROW_SHIFT(1);
			}
			
			// Make sure it's not just an entire line of zeros
			if(GET_SHIFT_ROW(shift - delta_shift) != 3) {
				grid = shiftingColumnDown(grid, shift, delta_shift);
			}
		}
	}
	
	// Now try to merge tiles
	for(int shift = MAKE_SHIFT(3, 3); shift >= MAKE_SHIFT(0, 0); shift = MAKE_LEFT(shift)) {
		unsigned tile = EXTRACT_TILE(grid, shift);
		if(tile != TILE_EMPTY && tile == EXTRACT_TILE(grid, MAKE_UP(shift))) {
			/*
			 *   X A A X -> X 0 0 X
			 *   X B A X -> X A 0 X
			 *   X C A X -> X B B X
			 *   X C A X -> X D B X
			 */
			grid = applyingTile(grid, shift, tile + 1);
			grid = shiftingColumnDown(grid, MAKE_UP(shift));
		}
	}
	
	return grid;
}

static inline CompressedGrid shiftingTilesLeft(CompressedGrid grid) {
	// Iterate through each tile from top to bottom and then left to right within each row
	for(int shift = MAKE_SHIFT(0, 0); shift < MAKE_SHIFT(4, 0); shift = MAKE_RIGHT(shift)) {
		// Only run for tiles in the left three columns
		if(GET_SHIFT_COL(shift) == 3) {
			continue;
		}
		
		// First skip past empty spaces
		unsigned tile = EXTRACT_TILE(grid, shift);
		if(tile == TILE_EMPTY) {
			/*
			 *   X X X X -> X X X X
			 *   0 0 0 0 -> 0 0 0 0
			 *   0 0 A B -> A B 0 0
			 *   X X X X -> X X X X
			 */
			
			// Count how many zeros we're shifting past
			int delta_shift = MAKE_COL_SHIFT(1);
			while(GET_SHIFT_COL(shift + delta_shift) != 0
			      && EXTRACT_TILE(grid, shift + delta_shift) == TILE_EMPTY
			) {
				delta_shift += MAKE_COL_SHIFT(1);
			}
			
			// Make sure it's not just an entire line of zeros
			if(GET_SHIFT_COL(shift + delta_shift) != 0) {
				grid = shiftingRowLeft(grid, shift, delta_shift);
			}
		}
	}
	
	// Now try to merge tiles
	for(int shift = MAKE_SHIFT(0, 0); shift < MAKE_SHIFT(4, 0); shift = MAKE_RIGHT(shift)) {
		// Only run for tiles in the left three columns
		if(GET_SHIFT_COL(shift) == 3) {
			continue;
		}
		
		unsigned tile = EXTRACT_TILE(grid, shift);
		if(tile != TILE_EMPTY && tile == EXTRACT_TILE(grid, MAKE_RIGHT(shift))) {
			/*
			 *   X X X X -> X X X X
			 *   A A A A -> B B 0 0
			 *   A A B C -> B B C 0
			 *   X X X X -> X X X X
			 */
			grid = applyingTile(grid, shift, tile + 1);
			grid = shiftingRowLeft(grid, MAKE_RIGHT(shift));
		}
	}
	
	return grid;
}


static inline CompressedGrid shiftingTilesRight(CompressedGrid grid) {
	// Iterate through each tile from bottom to top and then right to left within each row
	for(int shift = MAKE_SHIFT(3, 3); shift >= MAKE_SHIFT(0, 0); shift = MAKE_LEFT(shift)) {
		// Only run for tiles in the right three columns
		if(GET_SHIFT_COL(shift) == 0) {
			continue;
		}
		
		// First skip past empty spaces
		unsigned tile = EXTRACT_TILE(grid, shift);
		if(tile == TILE_EMPTY) {
			/*
			 *   X X X X -> X X X X
			 *   0 0 0 0 -> 0 0 0 0
			 *   A B 0 0 -> 0 0 A B
			 *   X X X X -> X X X X
			 */
			
			// Count how many zeros we're shifting past
			int delta_shift = MAKE_COL_SHIFT(1);
			while(GET_SHIFT_COL(shift - delta_shift) != 3
			      && EXTRACT_TILE(grid, shift - delta_shift) == TILE_EMPTY
			) {
				delta_shift += MAKE_COL_SHIFT(1);
			}
			
			// Make sure it's not just an entire line of zeros
			if(GET_SHIFT_COL(shift - delta_shift) != 3) {
				grid = shiftingRowRight(grid, shift, delta_shift);
			}
		}
	}
	
	// Now try to merge tiles
	for(int shift = MAKE_SHIFT(3, 3); shift >= MAKE_SHIFT(0, 0); shift = MAKE_LEFT(shift)) {
		// Only run for tiles in the right three columns
		if(GET_SHIFT_COL(shift) == 0) {
			continue;
		}
		
		unsigned tile = EXTRACT_TILE(grid, shift);
		if(tile != TILE_EMPTY && tile == EXTRACT_TILE(grid, MAKE_LEFT(shift))) {
			/*
			 *   X X X X -> X X X X
			 *   A A A A -> 0 0 B B
			 *   A B C C -> 0 A B D
			 *   X X X X -> X X X X
			 */
			grid = applyingTile(grid, shift, tile + 1);
			grid = shiftingRowRight(grid, MAKE_LEFT(shift));
		}
	}
	
	return grid;
}


Board::Board()
: mCompressedGrid(0) { }


void Board::placeTile(Tile tile, unsigned row, unsigned col) {
	mCompressedGrid = settingTile(mCompressedGrid, row, col, tile);
}


unsigned Board::findHoles(int* holeShifts) const {
	// Operate on a local copy of the grid, hopefully in a register
	uint64_t grid = mCompressedGrid;
	
	// Find all of the holes into which a tile can be placed
	unsigned holeCount = 0;
	for(int shift = MAKE_SHIFT(0, 0); shift <= MAKE_SHIFT(3, 3); shift = MAKE_RIGHT(shift)) {
		if(EXTRACT_TILE(grid, shift) == TILE_EMPTY) {
			holeShifts[holeCount++] = shift;
		}
	}
	
	return holeCount;
}


void Board::placeRandom(unsigned* pRow, unsigned* pCol, Tile* pTile) {
	int holeShifts[16];
	unsigned holeCount = findHoles(holeShifts);
	
	// Generate random tile value (10% chance of spawning a 4)...
	unsigned tile = 1 << (rand() % 10 == 0);
	if(pTile != nullptr) {
		*pTile = tile;
	}
	
	// ...and pick which hole to place it in with even distribution.
	int hole = holeShifts[rand() % holeCount];
	if(pRow != nullptr) {
		*pRow = GET_SHIFT_ROW(hole);
		*pCol = GET_SHIFT_COL(hole);
	}
	
	// Set new tile
	mCompressedGrid = applyingTile(mCompressedGrid, hole, tile);
}


bool Board::shiftTiles(Direction dir) {
	switch(dir) {
		case Direction::UP:
			return shiftTilesUp();
			
		case Direction::DOWN:
			return shiftTilesDown();
			
		case Direction::LEFT:
			return shiftTilesLeft();
			
		case Direction::RIGHT:
			return shiftTilesRight();
	}
}

bool Board::shiftTilesUp() {
	CompressedGrid old = mCompressedGrid;
	mCompressedGrid = shiftingTilesUp(old);
	return mCompressedGrid != old;
}

bool Board::shiftTilesDown() {
	CompressedGrid old = mCompressedGrid;
	mCompressedGrid = shiftingTilesDown(old);
	return mCompressedGrid != old;
}

bool Board::shiftTilesLeft() {
	CompressedGrid old = mCompressedGrid;
	mCompressedGrid = shiftingTilesLeft(old);
	return mCompressedGrid != old;
}

bool Board::shiftTilesRight() {
	CompressedGrid old = mCompressedGrid;
	mCompressedGrid = shiftingTilesRight(old);
	return mCompressedGrid != old;
}


bool Board::isGameOver() const {
	CompressedGrid grid = mCompressedGrid;
	
	if(shiftingTilesUp(grid) != grid) {
		return false;
	}
	
	if(shiftingTilesDown(grid) != grid) {
		return false;
	}
	
	if(shiftingTilesLeft(grid) != grid) {
		return false;
	}
	
	if(shiftingTilesRight(grid) != grid) {
		return false;
	}
	
	return true;
}


void Board::print() const {
	// Operate on a local copy of the grid, hopefully in a register
	CompressedGrid grid = mCompressedGrid;
	
	/*
	 *         |       |       |
	 *       2 | 65536 |     4 |     8
	 *         |       |       |
	 *  -------+-------+-------+-------
	 *         |       |       |
	 *    2048 |       |  4096 |    32
	 *         |       |       |
	 *  -------+-------+-------+-------
	 *         |       |       |
	 *    4096 |       |       |    64
	 *         |       |       |
	 *  -------+-------+-------+-------
	 *         |       |       |
	 *    8192 | 32768 |  1024 |   128
	 *         |       |       |
	 *
	 */
	for(unsigned i = 0; i < 4; ++i) {
		if(i > 0) {
			printf("-------+-------+-------+-------\n");
		}
		
		printf("       |       |       |       \n");
		
		// Print each tile in the row
		for(unsigned j = 0; j < 4; ++j) {
			if(j > 0) {
				printf("|");
			}
			
			unsigned val = TILE_VALUE(GET_TILE(grid, i, j));
			if(val == 1) {
				printf("       ");
			}
			else {
				printf(" %5u ", val);
			}
		}
		printf("\n");
		
		// Spacer at bottom of row
		printf("       |       |       |       \n");
	}
	
	printf("\n\n");
}


static inline void getLargestTiles(
	CompressedGrid grid,
	unsigned* row1,
	unsigned* col1,
	Tile* tile1,
	unsigned* row2,
	unsigned* col2,
	Tile* tile2
) {
	unsigned r1, c1, r2, c2;
	r1 = c1 = r2 = c2 = 0;
	Tile big1 = TILE_EMPTY, big2 = TILE_EMPTY;
	for(int shift = MAKE_SHIFT(0, 0); shift <= MAKE_SHIFT(3, 3); shift = MAKE_RIGHT(shift)) {
		Tile cur = EXTRACT_TILE(grid, shift);
		if(cur >= big1) {
			r2 = GET_SHIFT_ROW(shift);
			c2 = GET_SHIFT_COL(shift);
			big2 = cur;
		}
		if(cur > big1) {
			r1 = GET_SHIFT_ROW(shift);
			c1 = GET_SHIFT_COL(shift);
			big1 = cur;
		}
	}
	
	*row1 = r1;
	*col1 = c1;
	*tile1 = big1;
	*row2 = r2;
	*col2 = c2;
	*tile2 = big2;
}


static inline CompressedGrid rotateGrid(CompressedGrid grid) {
	CompressedGrid ret = GRID_EMPTY;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			Tile tile = GET_TILE(grid, r, c);
			ret = settingTile(ret, c, 4 - r - 1, tile);
		}
	}
	return ret;
}


static inline CompressedGrid mirrorGrid(CompressedGrid grid) {
	CompressedGrid l = grid & 0xff00ff00ff00ff00;
	CompressedGrid r = grid & 0x00ff00ff00ff00ff;
	grid = (r << 8) | (l >> 8);
	
	l = grid & 0xf0f0f0f0f0f0f0f0;
	r = grid & 0x0f0f0f0f0f0f0f0f;
	grid = (r << 4) | (l >> 4);
	
	return grid;
}


static inline void foreachOrientation(CompressedGrid grid, std::function<void(CompressedGrid)> func) {
	// XXX: Instead of this, it would be faster to come up with a method for deciding on a standard board layout
	func(grid);
	func(mirrorGrid(grid));
	
	func((grid = rotateGrid(grid)));
	func(mirrorGrid(grid));
	
	func((grid = rotateGrid(grid)));
	func(mirrorGrid(grid));
	
	func((grid = rotateGrid(grid)));
	func(mirrorGrid(grid));
}


static inline int scoreLayout(CompressedGrid grid) {
	int score = 0;
	/*
	 * Scoring Criteria
	 *
	 * 1. Highest tile value on the board
	 * 2. Negated manhattan distance between largest tile and the top-left corner
	 * 3. Negated manhattan distance between largest tile and second largest
	 */
	
	// 1. Highest tile value on the board
	Tile tile1, tile2;
	unsigned row1, col1, row2, col2;
	getLargestTiles(grid, &row1, &col1, &tile1, &row2, &col2, &tile2);
	score |= tile1;
	
	// 2. Negated manhattan distance between largest tile and the top-left corner
	score <<= 3;
	score |= 0x7 - (row1 + col1);
	
	// 3. Negated manhattan distance between largest tile and second largest
	score <<= 3;
	int dr, dc;
	dr = (int)row2 - row1;
	if(dr < 0) {
		dr = -dr;
	}
	dc = (int)col2 - col1;
	if(dc < 0) {
		dc = -dc;
	}
	score |= 0x7 - (dr + dc);
	
	return score;
}


int Board::estimateScore() const {
	if(isGameOver()) {
		return -999999;
	}
	
	int best = INT_MIN;
	foreachOrientation(mCompressedGrid, [&best](CompressedGrid grid) {
		int score = scoreLayout(grid);
		if(score > best) {
			best = score;
		}
	});
	return best;
}


void Board::allPlaces(Board* places) const {
	CompressedGrid grid = mCompressedGrid;
	for(int shift = MAKE_SHIFT(0, 0); shift <= MAKE_SHIFT(3, 3); shift = MAKE_RIGHT(shift)) {
		Tile cur = EXTRACT_TILE(grid, shift);
		if(cur == TILE_EMPTY) {
			places++->mCompressedGrid = settingTile(grid, GET_SHIFT_ROW(shift), GET_SHIFT_COL(shift), TILE_2);
			places++->mCompressedGrid = settingTile(grid, GET_SHIFT_ROW(shift), GET_SHIFT_COL(shift), TILE_4);
		}
		else {
			places++->mCompressedGrid = GRID_EMPTY;
			places++->mCompressedGrid = GRID_EMPTY;
		}
	}
}

void Board::allShifts(Board* up, Board* down, Board* left, Board* right) const {
	CompressedGrid grid = mCompressedGrid;
	up->mCompressedGrid = shiftingTilesUp(grid);
	if(up->mCompressedGrid == grid) {
		up->mCompressedGrid = GRID_EMPTY;
	}
	
	down->mCompressedGrid = shiftingTilesDown(grid);
	if(down->mCompressedGrid == grid) {
		down->mCompressedGrid = GRID_EMPTY;
	}
	
	left->mCompressedGrid = shiftingTilesLeft(grid);
	if(left->mCompressedGrid == grid) {
		left->mCompressedGrid = GRID_EMPTY;
	}
	
	right->mCompressedGrid = shiftingTilesRight(grid);
	if(right->mCompressedGrid == grid) {
		right->mCompressedGrid = GRID_EMPTY;
	}
}


bool Board::isEmpty() const {
	return mCompressedGrid == GRID_EMPTY;
}
