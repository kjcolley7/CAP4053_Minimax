//
//  GameEngine.cpp
//
//  Created by Kevin Colley on 10/5/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#include "GameEngine.h"


GameEngine::GameEngine(const std::string& windowTitle, unsigned width, unsigned height)
: mWindow(sf::VideoMode(width, height), windowTitle, sf::Style::Titlebar | sf::Style::Close),
  mLevel(mWindow)
{
	mWindow.setVerticalSyncEnabled(true);
	mWindow.setKeyRepeatEnabled(false);
}

int GameEngine::run() {
	sf::Clock timer;
	
	while(mWindow.isOpen()) {
		sf::Event event;
		if(mWindow.pollEvent(event)) {
			switch(event.type) {
				case sf::Event::Closed:
					mWindow.close();
					break;
					
				case sf::Event::KeyPressed:
					if(event.key.code == sf::Keyboard::Escape) {
						mWindow.close();
						break;
					}
					
					mLevel.keyPressed(event.key.code);
					break;
				
				case sf::Event::KeyReleased:
					mLevel.keyReleased(event.key.code);
					break;
				
				case sf::Event::MouseButtonPressed:
					if(event.mouseButton.button == sf::Mouse::Left) {
						mLevel.mouseButtonPressed(event.mouseButton.x, event.mouseButton.y);
					}
					break;
				
				case sf::Event::MouseButtonReleased:
					if(event.mouseButton.button == sf::Mouse::Left) {
						mLevel.mouseButtonReleased(event.mouseButton.x, event.mouseButton.y);
					}
					break;
				
				case sf::Event::MouseMoved:
					mLevel.mouseMoved(event.mouseMove.x, event.mouseMove.y);
					break;
					
				default:
					break;
			}
		}
		
		float deltaTime = timer.restart().asSeconds();
		mLevel.update(deltaTime);
		mLevel.draw(deltaTime);
		
		mWindow.display();
		mWindow.clear(sf::Color::Black);
	}
	
	return 0;
}
