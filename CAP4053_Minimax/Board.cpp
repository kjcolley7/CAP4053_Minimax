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


uint16_t Board::shiftTable[65536];
int Board::scoreTable[65536];


static inline CompressedGrid insertingTile(CompressedGrid grid, int shift, CompressedGrid tile) {
	return grid | (tile << shift);
}

static inline CompressedGrid placingTile(CompressedGrid grid, unsigned row, unsigned col, CompressedGrid tile) {
	return insertingTile(grid, MAKE_SHIFT(row, col), tile);
}

static inline CompressedGrid applyingTile(CompressedGrid grid, int shift, CompressedGrid tile) {
	return ((grid & ~(TILE_MASK << shift)) | (tile << shift));
}

static inline CompressedGrid settingTile(CompressedGrid grid, unsigned row, unsigned col, CompressedGrid tile) {
	return applyingTile(grid, MAKE_SHIFT(row, col), tile);
}


static inline uint16_t extractRow(CompressedGrid grid, unsigned row) {
	return (grid >> MAKE_ROW_SHIFT(row)) & 0xffff;
}

static inline uint16_t extractCol(CompressedGrid grid, unsigned col) {
	uint16_t ret = 0;
	for(int shift = MAKE_SHIFT(0, col); shift < 64; shift = MAKE_DOWN(shift)) {
		ret |= EXTRACT_TILE(grid, shift) << TILE_BITS * GET_SHIFT_ROW(shift);
	}
	return ret;
}

static inline CompressedGrid settingRow(CompressedGrid grid, unsigned row, uint16_t line) {
	grid &= ~((CompressedGrid)0xffff << MAKE_ROW_SHIFT(row));
	grid |= (CompressedGrid)line << MAKE_ROW_SHIFT(row);
	return grid;
}

static inline CompressedGrid settingColumn(CompressedGrid grid, unsigned col, uint16_t line) {
	grid &= ~(0x000f000f000f000f << MAKE_COL_SHIFT(col));
	for(unsigned row = 0; row < 4; row++) {
		grid = placingTile(grid, row, col, line & TILE_MASK);
		line >>= TILE_BITS;
	}
	return grid;
}


static inline uint16_t shiftingLine(uint16_t line, int dst, int delta = MAKE_COL_SHIFT(1)) {
	for(int src = dst + delta;
		GET_SHIFT_COL(dst) < GET_SHIFT_COL(src);
		dst = MAKE_RIGHT(dst), src = dst + delta
	) {
		line = (line & ~(TILE_MASK << dst)) | (EXTRACT_TILE(line, src) << dst);
	}
	for(; GET_SHIFT_COL(dst) != 0; dst = MAKE_RIGHT(dst)) {
		line = (line & ~(TILE_MASK << dst));
	}
	return line;
}


void Board::fillShiftTable() {
	for(uint32_t line = 0; line < 65536; ++line) {
		uint16_t cur = line;
		
		// Skip past all holes
		for(int shift = 0; shift < (4 * TILE_BITS); shift += TILE_BITS) {
			Tile tile = EXTRACT_TILE(cur, shift);
			if(tile == TILE_EMPTY) {
				int delta_shift = TILE_BITS;
				while(GET_SHIFT_COL(shift + delta_shift) != 0 &&
				      EXTRACT_TILE(cur, shift + delta_shift) == TILE_EMPTY
				) {
					delta_shift += TILE_BITS;
				}
				
				// Make sure it's not just an entire line of zeros
				if(GET_SHIFT_COL(shift + delta_shift) != 0) {
					cur = shiftingLine(cur, shift, delta_shift);
				}
			}
		}
		
		// Only run for tiles in the first three slots
		for(int shift = 0; shift < (3 * TILE_BITS); shift += TILE_BITS) {
			unsigned tile = EXTRACT_TILE(cur, shift);
			if(tile != TILE_EMPTY && tile == EXTRACT_TILE(cur, shift + TILE_BITS)) {
				/*
				 *   X X X X -> X X X X
				 *   A A A A -> B B 0 0
				 *   A A B C -> B B C 0
				 *   X X X X -> X X X X
				 */
				cur = (cur & ~(TILE_MASK << shift)) | ((tile + 1) << shift);
				cur = shiftingLine(cur, shift + TILE_BITS);
			}
		}
		
		// Store result of shift
		shiftTable[line] = cur;
	}
}


static inline uint16_t reversingLine(uint16_t line) {
	line = ((line & 0x00ff) << 8) | ((line & 0xff00) >> 8);
	line = ((line & 0x0f0f) << 4) | ((line & 0xf0f0) >> 4);
	return line;
}


static inline CompressedGrid shiftingTilesUp(CompressedGrid grid) {
	for(unsigned col = 0; col < 4; col++) {
		grid = settingColumn(grid, col, Board::shiftTable[extractCol(grid, col)]);
	}
	return grid;
}


static inline CompressedGrid shiftingTilesDown(CompressedGrid grid) {
	for(unsigned col = 0; col < 4; col++) {
		grid = settingColumn(grid, col, reversingLine(Board::shiftTable[reversingLine(extractCol(grid, col))]));
	}
	return grid;
}

static inline CompressedGrid shiftingTilesLeft(CompressedGrid grid) {
	for(unsigned row = 0; row < 4; row++) {
		grid = settingRow(grid, row, Board::shiftTable[extractRow(grid, row)]);
	}
	return grid;
}


static inline CompressedGrid shiftingTilesRight(CompressedGrid grid) {
	for(unsigned row = 0; row < 4; row++) {
		grid = settingRow(grid, row, reversingLine(Board::shiftTable[reversingLine(extractRow(grid, row))]));
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
	mCompressedGrid = insertingTile(mCompressedGrid, hole, tile);
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


static inline int scoreLine(uint_fast8_t* row) {
	int score = 0;
	
	// Find biggest tile and its index in the line
	Tile big = TILE_EMPTY;
	int bigIdx = 0;
	for(int i = 0; i < 4; i++) {
		Tile cur = row[i];
		if(cur > big) {
			big = cur;
			bigIdx = i;
		}
		
		if(cur == TILE_EMPTY) {
			// It's good to have empty tiles
			score += 500;
		}
	}
	
	// Best if the largest tile is on the edge
	if(bigIdx == 0 || bigIdx == 3) {
		score += 2400;
	}
	
	// Check for tiles that can almost be merged
	for(int i = 0; i < 3; i++) {
		if(row[i] == row[i+1] + 1 || row[i] == row[i+1] - 1) {
			score += 80;
		}
	}
	
	// Check if the tiles are lined up
	if((row[0] > row[1] && row[1] > row[2] && row[2] > row[3]) ||
	   (row[0] < row[1] && row[1] < row[2] && row[2] < row[3])
	) {
		score += 1500;
	}
	
	return score;
}


void Board::fillScoreTable() {
	for(uint32_t line = 0; line < 65536; line++) {
		uint16_t cur = line;
		
		// Build an array for the scoring function
		uint_fast8_t slots[4];
		for(int i = 0; i < 4; i++) {
			slots[i] = cur & TILE_MASK;
			if(slots[i] != TILE_EMPTY) {
				++slots[i];
			}
			cur >>= TILE_BITS;
		}
		
		// Score the slots and save it in the table
		scoreTable[line] = scoreLine(slots);
	}
}


int Board::estimateScore() const {
	if(isGameOver()) {
		return -999999;
	}
	
	CompressedGrid grid = mCompressedGrid;
	int score = 0;
	
	// Score horizontal stripes
	for(int r = 0; r < 4; r++) {
		int rowScore = Board::scoreTable[extractRow(grid, r)];
		score += rowScore;
	}
	
	// Score vertical stripes
	for(int c = 0; c < 4; c++) {
		int colScore = Board::scoreTable[extractCol(grid, c)];
		score += colScore;
	}
	
	return score;
}


void Board::allPlaces(Board* places) const {
	CompressedGrid grid = mCompressedGrid;
	for(int shift = MAKE_SHIFT(0, 0); shift <= MAKE_SHIFT(3, 3); shift = MAKE_RIGHT(shift)) {
		Tile cur = EXTRACT_TILE(grid, shift);
		if(cur == TILE_EMPTY) {
			places++->mCompressedGrid = insertingTile(grid, shift, TILE_2);
			places++->mCompressedGrid = insertingTile(grid, shift, TILE_4);
		}
		else {
			places++->mCompressedGrid = GRID_EMPTY;
			places++->mCompressedGrid = GRID_EMPTY;
		}
	}
}

void Board::allShifts(Board* shifts) const {
	CompressedGrid grid = mCompressedGrid;
	shifts->mCompressedGrid = shiftingTilesUp(grid);
	if(shifts->mCompressedGrid == grid) {
		shifts->mCompressedGrid = GRID_EMPTY;
	}
	
	++shifts;
	shifts->mCompressedGrid = shiftingTilesDown(grid);
	if(shifts->mCompressedGrid == grid) {
		shifts->mCompressedGrid = GRID_EMPTY;
	}
	
	++shifts;
	shifts->mCompressedGrid = shiftingTilesLeft(grid);
	if(shifts->mCompressedGrid == grid) {
		shifts->mCompressedGrid = GRID_EMPTY;
	}
	
	++shifts;
	shifts->mCompressedGrid = shiftingTilesRight(grid);
	if(shifts->mCompressedGrid == grid) {
		shifts->mCompressedGrid = GRID_EMPTY;
	}
}


bool Board::isEmpty() const {
	return mCompressedGrid == GRID_EMPTY;
}
