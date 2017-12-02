//
//  BoardPrivate.h
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 11/27/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#ifndef MM_BOARDPRIVATE_H
#define MM_BOARDPRIVATE_H

#include <cstdint>

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

#endif /* MM_BOARDPRIVATE_H */
