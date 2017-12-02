//
//  Level.cpp
//  CAP4053_Minimax
//
//  Created by kTeam on 11/19/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "Level.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "helpers.h"


Level::Level(sf::RenderTarget& target)
: mCanvas(target), mTextures(std::make_shared<TextureAtlas>(resourcePath() + "sprites.pack")) {
	// Load font
	mFont = std::make_shared<sf::Font>();
	if(!mFont->loadFromFile(resourcePath() + "ClearSans-Bold.ttf")) {
		std::cerr << "Failed to load font file" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// Create the board and place first two random tiles
	mBoard = std::make_shared<DrawableBoard>(mFont, mTextures);
	mBoard->placeRandom();
	mBoard->placeRandom();
	
	// Move the board into position (centered at the bottom of the window)
	float distanceToEdge = (600 - mBoard->kBoardWidth) / 2.0f;
	float halfBoard = mBoard->kBoardWidth / 2.0f;
	mBoard->setCenter({600 / 2.0f, 800 - distanceToEdge - halfBoard});
}

void Level::update(float deltaTime) {
	// Nothing to be done
}

void Level::draw(float deltaTime) {
	mBoard->draw(mCanvas);
}

void Level::keyPressed(sf::Keyboard::Key key) {
	bool didMove = false;
	switch(key) {
		case sf::Keyboard::Up:
		case sf::Keyboard::W:
		case sf::Keyboard::K:
			didMove = mBoard->shiftTilesUp();
			break;
		
		case sf::Keyboard::Left:
		case sf::Keyboard::A:
		case sf::Keyboard::H:
			didMove = mBoard->shiftTilesLeft();
			break;
		
		case sf::Keyboard::Down:
		case sf::Keyboard::S:
		case sf::Keyboard::J:
			didMove = mBoard->shiftTilesDown();
			break;
		
		case sf::Keyboard::Right:
		case sf::Keyboard::D:
		case sf::Keyboard::L:
			didMove = mBoard->shiftTilesRight();
			break;
		
		default:
			break;
	}
	
	if(didMove) {
		mBoard->placeRandom();
		mBoard->print();
		if(mBoard->isGameOver()) {
			// TODO: Display as text in the game window
			std::cout << "Game over!" << std::endl;
		}
	}
}

void Level::keyReleased(sf::Keyboard::Key key) {
	// Nothing to do
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
