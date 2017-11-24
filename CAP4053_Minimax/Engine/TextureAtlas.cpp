//
//  TextureAtlas.cpp
//
//  Created by Kevin Colley on 10/5/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#include "TextureAtlas.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include "helpers.h"


TextureAtlas::TextureAtlas(const std::string& atlasFilename) {
	// Open texture atlas file
	std::ifstream atlas{atlasFilename};
	if(!atlas) {
		std::cerr << "Failed to open texture atlas " << atlasFilename << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// Get path to directory containing this texture atlas file
	size_t i = atlasFilename.size();
	while(atlasFilename[i-1] != '/' && atlasFilename[i-1] != '\\') {
		--i;
	}
	std::string atlasDir = atlasFilename.substr(0, i);
	
	// Keep parsing pages until there are no more
	while(parsePage(atlas, atlasDir)) { }
}

bool TextureAtlas::parsePage(std::istream& atlas, const std::string& atlasDir) {
	std::string line = "";
	while(line.empty()) {
		if(!std::getline(atlas, line)) {
			return false;
		}
	}
	
	// Extract name of texture file
	std::string textureFileName = atlasDir + strip(line);
	
	// Load texture for this page
	mPages.emplace_back();
	sf::Texture& pageTex = *mPages.rbegin();
	if(!pageTex.loadFromFile(textureFileName)) {
		std::cerr << "Failed to load texture file " << textureFileName << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// Read texture page line by line
	while(std::getline(atlas, line)) {
		if(line.empty()) {
			// End of page
			return true;
		}
		
		// Try to parse the line as "option: value"
		std::string optionName, optionValue;
		if(parseOption(line, optionName, optionValue)) {
			// No sprite descriptor started yet, so store global option
			mOptions[optionName] = optionValue;
		}
		else {
			while(!line.empty()) {
				// Parse sprite name
				std::string spriteName = strip(line);
				
				// Create sprite
				sf::Sprite& sprite = mSprites[spriteName];
				sprite.setTexture(pageTex);
				
				// Parse sprite info to set its texture location
				if(!parseSprite(atlas, line, sprite)) {
					return false;
				}
			}
		}
	}
	
	return true;
}

bool TextureAtlas::parseSprite(std::istream& atlas, std::string& line, sf::Sprite& sprite) const {
	// Sprite options
	sf::Vector2i xy, size;
	
	// Parse each line
	while(std::getline(atlas, line)) {
		std::string optionName, optionValue;
		if(!parseOption(line, optionName, optionValue)) {
			break;
		}
		
		if(optionName == "xy") {
			if(!parseVector2i(optionValue, xy)) {
				return false;
			}
		}
		else if(optionName == "size") {
			if(!parseVector2i(optionValue, size)) {
				return false;
			}
		}
	}
	
	// Set sprite texture location
	sprite.setTextureRect(sf::IntRect{xy, size});
	return true;
}

bool TextureAtlas::parseOption(const std::string& line, std::string& name, std::string& value) const {
	// Check for lines containing options like "repeat: none"
	size_t colonIdx = line.find_first_of(':');
	if(colonIdx == std::string::npos) {
		return false;
	}
	
	size_t i = 0;
	
	// Skip leading whitespace "   repeat: none"
	while(std::isspace(line[i])) {
		++i;
	}
	size_t keyStart = i;
	
	// First non-whitespace character should match [a-zA-Z0-9_]
	if(!std::isalnum(line[keyStart]) && line[keyStart] != '_') {
		return false;
	}
	
	// Find end of token matching [a-zA-Z0-9_]+
	while(std::isalnum(line[i]) || line[i] == '_') {
		++i;
	}
	
	// Store option name
	name = line.substr(keyStart, i - keyStart);
	
	// Skip spaces between key name and colon
	while(std::isspace(line[i])) {
		++i;
	}
	
	// Make sure we see a colon
	if(line[i++] != ':') {
		std::cerr << "Expected ':' after '" << name << "'" << std::endl;
		return false;
	}
	
	// Skip spaces between colon and value
	while(std::isspace(line[i])) {
		++i;
	}
	size_t valueStart = i;
	
	// Trim trailing spaces
	i = line.size();
	while(std::isspace(line[i-1])) {
		--i;
	}
	
	// Store option value
	value = line.substr(valueStart, i - valueStart);
	return true;
}

bool TextureAtlas::parseVector2i(const std::string& str, sf::Vector2i& vector) const {
	size_t i = 0;
	
	// Skip leading whitespace
	while(std::isspace(str[i])) {
		++i;
	}
	size_t xStart = i;
	
	// Make sure X starts with a digit
	if(!std::isdigit(str[i])) {
		return false;
	}
	
	// Find end of X value
	while(std::isdigit(str[i])) {
		++i;
	}
	int x = std::atoi(str.substr(xStart, i - xStart).c_str());
	
	// Skip whitespace after X value
	while(std::isspace(str[i])) {
		++i;
	}
	
	// Check for comma
	if(str[i++] != ',') {
		return false;
	}
	
	// Skip whitespace between comma and Y value
	while(std::isspace(str[i])) {
		++i;
	}
	size_t yStart = i;
	
	// Make sure Y starts with a digit
	if(!std::isdigit(str[i])) {
		return false;
	}
	
	// Find end of Y value
	while(std::isdigit(str[i])) {
		++i;
	}
	int y = std::atoi(str.substr(yStart, i - yStart).c_str());
	
	// Skip spaces after Y value
	while(i < str.size() && std::isspace(str[i])) {
		++i;
	}
	
	// Make sure there are no non-whitespace characters after Y value
	if(i != str.size()) {
		return false;
	}
	
	// Store output vector
	vector.x = x;
	vector.y = y;
	return true;
}

bool TextureAtlas::getSprite(const std::string& spriteName, sf::Sprite& sprite) const {
	auto&& it = mSprites.find(spriteName);
	if(it == mSprites.cend()) {
		return false;
	}
	
	sprite = it->second;
	return true;
}
