//
//  Level.h
//  CAP4053_Minimax
//
//  Created by Kevin Colley on 11/19/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#ifndef MM_LEVEL_H
#define MM_LEVEL_H

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

class Level;
#include "TextureAtlas.h"
#include "Board.h"


/**
 * Bridge between events such as user input or the game loop and the game objects.
 */
class Level {
public:
	/**
	 * Constructs a level object given a reference to the game object and the canvas
	 * to which objects will be drawn.
	 * @param canvas Reference to the render target to which objects will be drawn
	 */
	Level(sf::RenderTarget& canvas);
	
	/**
	 * Update game objects while playing.
	 * @param deltaTime Time in seconds elapsed since last update call
	 */
	void update(float deltaTime);
	
	/**
	 * Draw game objects to the screen.
	 * @param deltaTime Time in seconds elapsed since last draw call
	 */
	void draw(float deltaTime);
	
	/**
	 * Called when a keyboard key is pressed.
	 * @param key The key that was pressed
	 */
	void keyPressed(sf::Keyboard::Key key);
	
	/**
	 * Called when a keyboard key is released.
	 * @param key The key that was released
	 */
	void keyReleased(sf::Keyboard::Key key);
	
	/**
	 * Called when the left mouse button is pressed.
	 * @param x X-coordinate of the cursor in window-space
	 * @param y Y-coordinate of the cursor in window-space
	 */
	void mouseButtonPressed(int x, int y);
	
	/**
	 * Called when the left mouse button is released.
	 * @param x X-coordinate of the cursor in window-space
	 * @param y Y-coordinate of the cursor in window-space
	 */
	void mouseButtonReleased(int x, int y);
	
	/**
	 * Called when the mouse's position changes.
	 * @param x X-coordinate of the cursor in window-space
	 * @param y Y-coordinate of the cursor in window-space
	 */
	void mouseMoved(int x, int y);
	
private:
	sf::RenderTarget& mCanvas;
	std::shared_ptr<TextureAtlas> mTextures;
	sf::Font mFont;
	Board mBoard;
};

#endif /* MM_LEVEL_H */
