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
	/*  4096 */ { 60,  58,  50},
	/*  8192 */ { 60,  58,  50},
	/* 16384 */ { 60,  58,  50},
	/* 32768 */ { 60,  58,  50}
};

const sf::Color DrawableBoard::kTileFontColors[16] = {
	/*     0 */ {},
	/*     2 */ {117, 110, 102},
	/*     4 */ {117, 110, 102},
	/*     8 */ {255, 255, 255},
	/*    16 */ {255, 255, 255},
	/*    32 */ {255, 255, 255},
	/*    64 */ {255, 255, 255},
	/*   128 */ {255, 255, 255},
	/*   256 */ {255, 255, 255},
	/*   512 */ {255, 255, 255},
	/*  1024 */ {255, 255, 255},
	/*  2048 */ {255, 255, 255},
	/*  4096 */ {255, 255, 255},
	/*  8192 */ {255, 255, 255},
	/* 16384 */ {255, 255, 255},
	/* 32768 */ {255, 255, 255}
};

const unsigned DrawableBoard::kTileFontSizes[16] = {
	/*     0 */ 0,
	/*     2 */ 55,
	/*     4 */ 55,
	/*     8 */ 55,
	/*    16 */ 55,
	/*    32 */ 55,
	/*    64 */ 55,
	/*   128 */ 45,
	/*   256 */ 45,
	/*   512 */ 45,
	/*  1024 */ 35,
	/*  2048 */ 35,
	/*  4096 */ 30,
	/*  8192 */ 30,
	/* 16384 */ 30,
	/* 32768 */ 30
};


DrawableBoard::DrawableBoard(std::shared_ptr<sf::Font> font, std::shared_ptr<TextureAtlas> textures)
: mFont(font), mBounds(0, 0, kBoardWidth, kBoardWidth) {
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
		// Get the tile value and use it to choose the tile's background color
		Tile tile = EXTRACT_TILE(mCompressedGrid, shift);
		mSprTile.setColor(kTileColors[tile]);
		
		sf::Vector2f tilePos = getSlotPosition(GET_SHIFT_ROW(shift), GET_SHIFT_COL(shift));
		mSprTile.setPosition(tilePos);
		canvas.draw(mSprTile);
		
		if(tile != TILE_EMPTY) {
			// Draw value number on tile using the font size for the tile's value
			std::string tileValue = std::to_string(TILE_VALUE(tile));
			sf::Text text{tileValue, *mFont, kTileFontSizes[tile]};
			text.setFillColor(kTileFontColors[tile]);
			
			// Position the text in the center of the tile
			sf::FloatRect box = text.getLocalBounds();
			text.setOrigin(box.left + box.width / 2.0f, box.top + box.height / 2.0f);
			text.setPosition(tilePos + (sf::Vector2f){kTileWidth / 2.0f, kTileWidth / 2.0f});
			
			// Draw it
			canvas.draw(text);
		}
	}
}
