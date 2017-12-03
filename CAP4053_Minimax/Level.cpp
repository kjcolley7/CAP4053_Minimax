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
: mCanvas(target), mTextures(std::make_shared<TextureAtlas>(resourcePath() + "sprites.pack")), mAIEnabled(false) {
	// Load font
	mFont = std::make_shared<sf::Font>();
	if(!mFont->loadFromFile(resourcePath() + "ClearSans-Bold.ttf")) {
		std::cerr << "Failed to load font file" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// Create the board, which will involve placing the first two random tiles
	mBoard = std::make_shared<DrawableBoard>(mFont, mTextures);
	mBoard->print();
	
	// Create AI
	mMinimax = std::make_unique<BoardTree>(*mBoard);
	
	// Move the board into position (centered at the bottom of the window)
	float distanceToEdge = (600 - mBoard->kBoardWidth) / 2.0f;
	float halfBoard = mBoard->kBoardWidth / 2.0f;
	mBoard->setCenter({600 / 2.0f, 800 - distanceToEdge - halfBoard});
	
	// Create text objects
	mTitle.setFont(*mFont);
	mTitle.setString("2048 AI");
	mTitle.setCharacterSize(80);
	mTitle.setFillColor({119, 110, 101});
	
	mInstructions.setFont(*mFont);
	mInstructions.setString(
		"Press SPACE to toggle the AI\n"
		"Controls are arrow keys, WASD, or HJKL\n"
		"Press R to restart the game"
	);
	mInstructions.setCharacterSize(24);
	mInstructions.setFillColor({119, 110, 101});
	
	// Position text objects
	sf::FloatRect titleBox = mTitle.getLocalBounds();
	mTitle.setOrigin(titleBox.left + titleBox.width / 2.0f, 0.0f);
	mTitle.setPosition(600 / 2.0f, 0.0f);
	
	sf::FloatRect instructionsBox = mInstructions.getLocalBounds();
	mInstructions.setOrigin(instructionsBox.left + instructionsBox.width / 2.0f, 0.0f);
	mInstructions.setPosition(600 / 2.0f, titleBox.top + titleBox.height + 50.0f);
}

void Level::update(float deltaTime) {
	if(mAIEnabled && !mBoard->checkGameOver()) {
		bool didMove = mBoard->shiftTiles(mMinimax->getBestMove());
		if(didMove) {
			unsigned row, col;
			Tile tile;
			mBoard->placeRandom(&row, &col, &tile);
			mMinimax->placedTile(row, col, tile);
			mBoard->print();
			if(mBoard->isGameOver()) {
				std::cout << "Game over!" << std::endl;
			}
		}
	}
}

void Level::draw(float deltaTime) {
	mCanvas.draw(mTitle);
	mCanvas.draw(mInstructions);
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
		
		case sf::Keyboard::R:
			mBoard->tryAgain();
			mMinimax->setBoard(*mBoard);
			break;
		
		case sf::Keyboard::Space:
			mAIEnabled = !mAIEnabled;
			if(mAIEnabled) {
				mMinimax->setBoard(*mBoard);
			}
			break;
		
		default:
			break;
	}
	
	if(didMove) {
		unsigned row, col;
		Tile tile;
		mBoard->placeRandom(&row, &col, &tile);
		if(mAIEnabled) {
			mMinimax->placedTile(row, col, tile);
		}
		mBoard->print();
		if(mBoard->isGameOver()) {
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
