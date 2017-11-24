//
//  Board.h
//  CAP4053_Minimax
//
//  Created by kTeam on 11/19/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#ifndef MM_BOARD_H
#define MM_BOARD_H

#include <cstdint>
#include "Direction.h"

#define TILE_EMPTY  0
#define TILE_2      1
#define TILE_4      2
#define TILE_8      3
#define TILE_16     4
#define TILE_32     5
#define TILE_64     6
#define TILE_128    7
#define TILE_256    8
#define TILE_512    9
#define TILE_1024  10
#define TILE_2048  11
#define TILE_4096  12
#define TILE_8192  13
#define TILE_16384 14
#define TILE_32768 15

typedef uint64_t CompressedGrid;


class Board {
public:
	Board();
	
	void placeTile(unsigned tile, unsigned row, unsigned col);
	void placeRandom();
	
	static CompressedGrid shiftingTilesUp(CompressedGrid grid);
	static CompressedGrid shiftingTilesDown(CompressedGrid grid);
	static CompressedGrid shiftingTilesLeft(CompressedGrid grid);
	static CompressedGrid shiftingTilesRight(CompressedGrid grid);
	
	void shiftTiles(Direction dir);
	void shiftTilesUp();
	void shiftTilesDown();
	void shiftTilesLeft();
	void shiftTilesRight();
	
	bool isGameOver() const;
	void print() const;

private:
	CompressedGrid mCompressedGrid;
};

#endif /* MM_BOARD_H */
