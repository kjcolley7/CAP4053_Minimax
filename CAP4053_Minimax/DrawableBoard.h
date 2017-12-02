//
//  DrawableBoard.h
//  CAP4053_Minimax
//
//  Created by kTeam on 11/27/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#ifndef MM_DRAWABLEBOARD_H
#define MM_DRAWABLEBOARD_H

#include <memory>
#include <SFML/Graphics.hpp>
#include "Engine/TextureAtlas.h"
#include "Board.h"


class DrawableBoard: public Board {
public:
	static const float kBoardWidth;
	
	DrawableBoard(std::shared_ptr<sf::Font> font, std::shared_ptr<TextureAtlas> textures);
	
	void setCenter(sf::Vector2f center);
	
	void upPressed();
	void downPressed();
	void leftPressed();
	void rightPressed();
	
	void draw(sf::RenderTarget& canvas);
	
private:
	sf::Vector2f getSlotPosition(unsigned row, unsigned col) const;
	
	static const float kDividerWidth, kTileWidth;
	static const sf::Color kTileColors[16];
	
	sf::FloatRect mBounds;
	sf::Font mFont;
	sf::Sprite mSprBackground;
	sf::Sprite mSprTile;
};

#endif /* MM_DRAWABLEBOARD_H */
