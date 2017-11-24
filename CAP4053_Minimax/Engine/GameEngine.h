//
//  GameEngine.h
//
//  Created by Kevin Colley on 10/5/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#ifndef KJC_GAMEENGINE_H
#define KJC_GAMEENGINE_H

#include <string>
#include <SFML/Graphics.hpp>

class GameEngine;
#include "Level.h"


/**
 * The main game engine class which handles OS interaction such as windowing and input events.
 */
class GameEngine {
public:
	/**
	 * Constructs a new game engine object and initializes the window.
	 * @param windowTitle Initial title for the window
	 */
	GameEngine(const std::string& windowTitle="My Game", unsigned width=640, unsigned height=480);
	
	/**
	 * Start the game's run loop and returns an exit code for main().
	 * @return Zero on success, nonzero otherwise
	 */
	int run();
	
private:
	sf::RenderWindow mWindow;
	Level mLevel;
};

#endif /* KJC_GAMEENGINE_H */
