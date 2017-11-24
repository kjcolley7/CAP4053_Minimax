//
//  Level.cpp
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 11/19/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#include "Level.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "helpers.h"


Level::Level(sf::RenderTarget& target)
: mCanvas(target), mTextures(std::make_shared<TextureAtlas>(resourcePath() + "tiles.pack")) {
	// Load font
	if(!mFont.loadFromFile(resourcePath() + "Roboto-Black.ttf")) {
		std::cerr << "Failed to load font file" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	//go do the game thing now plz
}

void Level::update(float deltaTime) {
	// Nothing to be done
}

void Level::draw(float deltaTime) {
	//TODO: Draw!
}

void Level::keyPressed(sf::Keyboard::Key key) {
	switch(key) {
		case sf::Keyboard::Up:
		case sf::Keyboard::W:
			mBoard.shiftTilesUp();
			break;
		
		case sf::Keyboard::Left:
		case sf::Keyboard::A:
			mBoard.shiftTilesLeft();
			break;
		
		case sf::Keyboard::Down:
		case sf::Keyboard::S:
			mBoard.shiftTilesDown();
			break;
		
		case sf::Keyboard::Right:
		case sf::Keyboard::D:
			mBoard.shiftTilesRight();
			break;
		
		default:
			break;
	}
}

void Level::keyReleased(sf::Keyboard::Key key) {
	switch(key) {
		case sf::Keyboard::Down:
			//TODO: Unduck!
			break;
		
		default:
			break;
	}
}

void Level::mouseButtonPressed(int x, int y) {
	// Nothing to do
}

void Level::mouseButtonReleased(int x, int y) {
	// Nothing to do
}

void Level::mouseMoved(int x, int y) {
	// Nothing to do
}
