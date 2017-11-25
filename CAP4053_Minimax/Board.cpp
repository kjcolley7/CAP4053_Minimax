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


/*
 * In the compressed grid format, each tile is stored in a nybble in row-major format. That
 * means the entire 4x4 grid can be stored in a single 8-byte uint64_t.
 */
#define TILE_BITS 4
#define TILE_MASK ((CompressedGrid)((1 << TILE_BITS) - 1))

#define GET_SHIFT_ROW(shift) (((shift) >> 4) & 0x3)
#define GET_SHIFT_COL(shift) (((shift) >> 2) & 0x3)
#define MAKE_ROW_SHIFT(row) (int)((row) << 4)
#define MAKE_COL_SHIFT(col) (int)((col) << 2)
#define MAKE_SHIFT(row, col) (MAKE_ROW_SHIFT(row) | MAKE_COL_SHIFT(col))
#define MAKE_UP(shift) (__typeof__(shift))((shift) - MAKE_ROW_SHIFT(1))
#define MAKE_DOWN(shift) (__typeof__(shift))((shift) + MAKE_ROW_SHIFT(1))
#define MAKE_LEFT(shift) (__typeof__(shift))((shift) - MAKE_COL_SHIFT(1))
#define MAKE_RIGHT(shift) (__typeof__(shift))((shift) + MAKE_COL_SHIFT(1))
#define EXTRACT_TILE(grid, shift) ((Tile)(((grid) >> (shift)) & TILE_MASK))
#define GET_TILE(grid, row, col) EXTRACT_TILE(grid, MAKE_SHIFT(row, col))
#define TILE_VALUE(tile) (1 << (uint_fast32_t)(tile))

static inline CompressedGrid applyingTile(CompressedGrid grid, int shift, CompressedGrid tile) {
	return ((grid & ~(TILE_MASK << shift)) | (tile << shift));
}

static inline CompressedGrid settingTile(CompressedGrid grid, unsigned row, unsigned col, int tile) {
	return applyingTile(grid, MAKE_SHIFT(row, col), tile);
}

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


void Board::placeTile(unsigned tile, unsigned row, unsigned col) {
	mCompressedGrid = settingTile(mCompressedGrid, row, col, tile);
}


void Board::placeRandom() {
	// Operate on a local copy of the grid, hopefully in a register
	uint64_t grid = mCompressedGrid;
	
	// Find all of the holes into which a tile can be placed
	unsigned holeCount = 0;
	int holeShifts[16];
	for(int shift = MAKE_SHIFT(0, 0); shift <= MAKE_SHIFT(3, 3); shift = MAKE_RIGHT(shift)) {
		if(EXTRACT_TILE(grid, shift) == TILE_EMPTY) {
			holeShifts[holeCount++] = shift;
		}
	}
	
	// Generate random tile value and pick which hole to place it in
	unsigned rnd = (unsigned)rand();
	unsigned tile = 1 << (rnd & 0x1);
	rnd >>= 1;
	int hole = holeShifts[rnd % holeCount];
	
	// Set new tile and store back in this class
	mCompressedGrid = applyingTile(grid, hole, tile);
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
	Board copy{*this};
	
	copy.shiftTilesUp();
	if(copy.mCompressedGrid != mCompressedGrid) {
		return false;
	}
	
	copy.shiftTilesDown();
	if(copy.mCompressedGrid != mCompressedGrid) {
		return false;
	}
	
	copy.shiftTilesLeft();
	if(copy.mCompressedGrid != mCompressedGrid) {
		return false;
	}
	
	copy.shiftTilesRight();
	if(copy.mCompressedGrid != mCompressedGrid) {
		return false;
	}
	
	return true;
}


void Board::print() const {
	// Operate on a local copy of the grid, hopefully in a register
	uint64_t grid = mCompressedGrid;
	
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
}
