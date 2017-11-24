//
//  Board.cpp
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 11/19/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#include "Board.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>


/*
 In the compressed grid format, each tile is stored in a nybble in row-major format. That
 means the entire 4x4 grid can be stored in a single 8-byte uint64_t.
 */
#define TILE_BITS 4
#define TILE_MASK ((CompressedGrid)((1 << TILE_BITS) - 1))
#define ROW_TILES 4
#define COL_TILES 4

#define GET_SHIFT_ROW(shift) (((shift) >> 4) & 0x3)
#define GET_SHIFT_COL(shift) (((shift) >> 2) & 0x3)
#define MAKE_ROW_SHIFT(row) (int)((row) << 4)
#define MAKE_COL_SHIFT(col) (int)((col) << 2)
#define MAKE_SHIFT(row, col) (MAKE_ROW_SHIFT(row) | MAKE_COL_SHIFT(col))
#define EXTRACT_TILE(grid, shift) (((grid) >> (shift)) & TILE_MASK)
#define GET_TILE(grid, row, col) EXTRACT_TILE(grid, MAKE_SHIFT(row, col))
#define TILE_VALUE(tile) (1 << (tile))
#define MAKE_UP(shift) (__typeof__(shift))((shift) - MAKE_ROW_SHIFT(1))
#define MAKE_DOWN(shift) (__typeof__(shift))((shift) + MAKE_ROW_SHIFT(1))
#define MAKE_LEFT(shift) (__typeof__(shift))((shift) - MAKE_COL_SHIFT(1))
#define MAKE_RIGHT(shift) (__typeof__(shift))((shift) + MAKE_COL_SHIFT(1))

#define APPLY_TILE(grid, shift, tile) ({ \
	int _shft = (shift); \
	(((grid) & ~(TILE_MASK << _shft)) | ((CompressedGrid)(tile) << _shft)); \
})

#define SET_TILE(grid, row, col, tile) ({ \
	int _shift = MAKE_SHIFT(row, col); \
	APPLY_TILE(grid, _shift, tile); \
})

#define SHIFT_COL_UP(grid, start, delta) ({ \
	CompressedGrid _grid = (grid); \
	int _delta = (int)(delta); \
	int _dst = (int)(start); \
	for(int _src = _dst + _delta; \
		GET_SHIFT_ROW(_dst) < GET_SHIFT_ROW(_src); \
		_dst = MAKE_DOWN(_dst), _src = _dst - _delta \
	) { \
		_grid = APPLY_TILE(_grid, _dst, EXTRACT_TILE(_grid, _src)); \
	} \
	for(; GET_SHIFT_ROW(_dst) != 0; _dst = MAKE_DOWN(_dst)) { \
		_grid = APPLY_TILE(_grid, _dst, TILE_EMPTY); \
	} \
	_grid; \
})

#define SHIFT_COL_DOWN(grid, start, delta) ({ \
	CompressedGrid _grid = (grid); \
	int _delta = (int)(delta); \
	int _dst = (int)(start); \
	for(int _src = _dst - _delta; \
		GET_SHIFT_ROW(_src) < GET_SHIFT_ROW(_dst); \
		_dst = MAKE_UP(_dst), _src = _dst - _delta \
	) { \
		_grid = APPLY_TILE(_grid, _dst, EXTRACT_TILE(_grid, _src)); \
	} \
	for(; GET_SHIFT_ROW(_dst) != 3; _dst = MAKE_UP(_dst)) { \
		_grid = APPLY_TILE(_grid, _dst, TILE_EMPTY); \
	} \
	_grid; \
})

#define SHIFT_ROW_LEFT(grid, start, delta) ({ \
	CompressedGrid _grid = (grid); \
	int _delta = (int)(delta); \
	int _dst = (int)(start); \
	for(int _src = _dst + _delta; \
		GET_SHIFT_COL(_dst) < GET_SHIFT_COL(_src); \
		_dst = MAKE_RIGHT(_dst), _src = _dst + _delta \
	) { \
		_grid = APPLY_TILE(_grid, _dst, EXTRACT_TILE(_grid, _src)); \
	} \
	for(; GET_SHIFT_COL(_dst) != 0; _dst = MAKE_RIGHT(_dst)) { \
		_grid = APPLY_TILE(_grid, _dst, TILE_EMPTY); \
	} \
	_grid; \
})

#define SHIFT_ROW_RIGHT(grid, start, delta) ({ \
	CompressedGrid _grid = (grid); \
	int _delta = (int)(delta); \
	int _dst = (int)(start); \
	for(int _src = _dst - _delta; \
		GET_SHIFT_COL(_src) < GET_SHIFT_COL(_dst); \
		_dst = MAKE_LEFT(_dst), _src = _dst - _delta \
	) { \
		_grid = APPLY_TILE(_grid, _dst, EXTRACT_TILE(_grid, _src)); \
	} \
	for(; GET_SHIFT_COL(_dst) != 3; _dst = MAKE_LEFT(_dst)) { \
		_grid = APPLY_TILE(_grid, _dst, TILE_EMPTY); \
	} \
	_grid; \
})

#define SHIFT_COL_UP_ONE(grid, shift) SHIFT_COL_UP(grid, shift, MAKE_ROW_SHIFT(1))
#define SHIFT_COL_DOWN_ONE(grid, shift) SHIFT_COL_DOWN(grid, shift, MAKE_ROW_SHIFT(1))
#define SHIFT_ROW_LEFT_ONE(grid, shift) SHIFT_ROW_LEFT(grid, shift, MAKE_COL_SHIFT(1))
#define SHIFT_ROW_RIGHT_ONE(grid, shift) SHIFT_ROW_RIGHT(grid, shift, MAKE_COL_SHIFT(1))


Board::Board()
: mCompressedGrid(0) { }


void Board::placeTile(unsigned tile, unsigned row, unsigned col) {
	mCompressedGrid = SET_TILE(mCompressedGrid, row, col, tile);
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
	mCompressedGrid = APPLY_TILE(grid, hole, tile);
}


CompressedGrid Board::shiftingTilesUp(CompressedGrid grid) {
	// Iterate through each tile from top to bottom and then left to right within each row
	for(int shift = MAKE_SHIFT(0, 0); shift < MAKE_SHIFT(3, 0); shift = MAKE_RIGHT(shift)) {
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
				printf("Shifting tiles up by %d from (%d, %d)\n", GET_SHIFT_ROW(delta_shift), GET_SHIFT_ROW(shift), GET_SHIFT_COL(shift));
				grid = SHIFT_COL_UP(grid, shift, delta_shift);
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
			grid = APPLY_TILE(grid, shift, tile + 1);
			grid = SHIFT_COL_UP_ONE(grid, MAKE_DOWN(shift));
		}
	}
	
	return grid;
}


CompressedGrid Board::shiftingTilesDown(CompressedGrid grid) {
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
				grid = SHIFT_COL_DOWN(grid, shift, delta_shift);
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
			grid = APPLY_TILE(grid, shift, tile + 1);
			grid = SHIFT_COL_DOWN_ONE(grid, MAKE_UP(shift));
		}
	}
	
	return grid;
}

CompressedGrid Board::shiftingTilesLeft(CompressedGrid grid) {
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
				grid = SHIFT_ROW_LEFT(grid, shift, delta_shift);
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
			grid = APPLY_TILE(grid, shift, tile + 1);
			grid = SHIFT_ROW_LEFT_ONE(grid, MAKE_RIGHT(shift));
		}
	}
	
	return grid;
}


CompressedGrid Board::shiftingTilesRight(CompressedGrid grid) {
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
				grid = SHIFT_ROW_RIGHT(grid, shift, delta_shift);
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
			grid = APPLY_TILE(grid, shift, tile + 1);
			grid = SHIFT_ROW_RIGHT_ONE(grid, MAKE_LEFT(shift));
		}
	}
	
	return grid;
}


void Board::shiftTiles(Direction dir) {
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

void Board::shiftTilesUp() {
	mCompressedGrid = shiftingTilesUp(mCompressedGrid);
}

void Board::shiftTilesDown() {
	mCompressedGrid = shiftingTilesDown(mCompressedGrid);
}

void Board::shiftTilesLeft() {
	mCompressedGrid = shiftingTilesLeft(mCompressedGrid);
}

void Board::shiftTilesRight() {
	mCompressedGrid = shiftingTilesRight(mCompressedGrid);
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
