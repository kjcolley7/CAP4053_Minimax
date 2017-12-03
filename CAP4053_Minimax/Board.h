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

typedef uint64_t CompressedGrid;
#define GRID_EMPTY  ((CompressedGrid)0)

typedef uint_fast8_t Tile;
#define TILE_EMPTY  ((Tile)0)
#define TILE_2      ((Tile)1)
#define TILE_4      ((Tile)2)
#define TILE_8      ((Tile)3)
#define TILE_16     ((Tile)4)
#define TILE_32     ((Tile)5)
#define TILE_64     ((Tile)6)
#define TILE_128    ((Tile)7)
#define TILE_256    ((Tile)8)
#define TILE_512    ((Tile)9)
#define TILE_1024   ((Tile)10)
#define TILE_2048   ((Tile)11)
#define TILE_4096   ((Tile)12)
#define TILE_8192   ((Tile)13)
#define TILE_16384  ((Tile)14)
#define TILE_32768  ((Tile)15)


class Board {
public:
	Board();
	
	void placeTile(Tile tile, unsigned row, unsigned col);
	unsigned findHoles(int* holeShifts) const;
	void placeRandom();
	
	bool shiftTiles(Direction dir);
	bool shiftTilesUp();
	bool shiftTilesDown();
	bool shiftTilesLeft();
	bool shiftTilesRight();
	
	bool isGameOver() const;
	void print() const;
	
	int estimateScore() const;

protected:
	CompressedGrid mCompressedGrid;
};

#endif /* MM_BOARD_H */
