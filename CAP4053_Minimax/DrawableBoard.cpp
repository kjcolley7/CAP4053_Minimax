//
//  DrawableBoard.cpp
//  CAP4053_Minimax
//
//  Created by kTeam on 11/27/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "DrawableBoard.h"
#include <iostream>
#include <cstdlib>
#include "Engine/helpers.h"
#include "BoardPrivate.h"


const float DrawableBoard::kBoardWidth = 500.0f;
const float DrawableBoard::kDividerWidth = 15.0f;
const float DrawableBoard::kTileWidth = (kBoardWidth - 5 * kDividerWidth) / 4.0f;

const sf::Color DrawableBoard::kTileColors[16] = {
	/*     0 */ {202, 193, 181},
	/*     2 */ {236, 228, 219},
	/*     4 */ {235, 224, 203},
	/*     8 */ {233, 179, 129},
	/*    16 */ {232, 153, 108},
	/*    32 */ {231, 130, 102},
	/*    64 */ {229, 103,  71},
	/*   128 */ {232, 207, 127},
	/*   256 */ {232, 204, 113},
	/*   512 */ {231, 200, 100},
	/*  1024 */ {231, 197,  89},
	/*  2048 */ {231, 195,  79},
	/*  4096 */ {},
	/*  8192 */ {},
	/* 16384 */ {},
	/* 32768 */ {}
};


DrawableBoard::DrawableBoard(std::shared_ptr<sf::Font> font, std::shared_ptr<TextureAtlas> textures)
: mBounds(0, 0, kBoardWidth, kBoardWidth) {
	if(!textures->getSprite("Background", mSprBackground) ||
	   !textures->getSprite("Tile", mSprTile)
	) {
		std::cerr << "Couldn't load board sprites!" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// Make sure the sprites are displayed with the intended dimensions
	setSize(mSprBackground, {kBoardWidth, kBoardWidth});
	setSize(mSprTile, {kTileWidth, kTileWidth});
}


void DrawableBoard::setCenter(sf::Vector2f center) {
	mBounds.left = center.x - mBounds.width / 2.0f;
	mBounds.top = center.y - mBounds.height / 2.0f;
	mSprBackground.setPosition(mBounds.left, mBounds.top);
}


void DrawableBoard::upPressed() {
	if(shiftTilesUp()) {
		// TODO: animate
	}
}


void DrawableBoard::downPressed() {
	if(shiftTilesDown()) {
		// TODO: animate
	}
}


void DrawableBoard::leftPressed() {
	if(shiftTilesLeft()) {
		// TODO: animate
	}
}


void DrawableBoard::rightPressed() {
	if(shiftTilesRight()) {
		// TODO: animate
	}
}


sf::Vector2f DrawableBoard::getSlotPosition(unsigned row, unsigned col) const {
	sf::Vector2f ret;
	ret.x = mBounds.left + (col + 1) * kDividerWidth + col * kTileWidth;
	ret.y = mBounds.top + (row + 1) * kDividerWidth + row * kTileWidth;
	return ret;
}


void DrawableBoard::draw(sf::RenderTarget& canvas) {
	// Draw the background grid
	canvas.draw(mSprBackground);
	
	// Draw each of the tiles
	for(int shift = MAKE_SHIFT(0, 0); shift <= MAKE_SHIFT(3, 3); shift = MAKE_RIGHT(shift)) {
		Tile tile = EXTRACT_TILE(mCompressedGrid, shift);
		mSprTile.setColor(kTileColors[tile]);
		mSprTile.setPosition(getSlotPosition(GET_SHIFT_ROW(shift), GET_SHIFT_COL(shift)));
		canvas.draw(mSprTile);
		
		// TODO: Draw value number on tile using font
	}
}
