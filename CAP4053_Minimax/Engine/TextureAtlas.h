//
//  TextureAtlas.h
//
//  Created by Kevin Colley on 10/5/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#ifndef KJC_TEXTUREATLAS_H
#define KJC_TEXTUREATLAS_H

#include <SFML/Graphics.hpp>
#include <string>
#include <istream>
#include <vector>
#include <unordered_map>

/**
 * Used for loading multiple sprites from the same texture.
 */
class TextureAtlas {
public:
	/**
	 * Construct a TextureAtlas object given a texture atlas file (.pack).
	 * @param atlasFilename File name of texture atlas file (.pack)
	 */
	TextureAtlas(const std::string& atlasFilename);
	
	/**
	 * Loads a named sprite from the texture atlas.
	 * @param spriteName Name of the sprite to load
	 * @param sprite Output reference where the sprite to load should be stored
	 * @return True on success, or false if the sprite couldn't be loaded
	 */
	bool getSprite(const std::string& spriteName, sf::Sprite& sprite) const;
	
private:
	bool parsePage(std::istream& atlas, const std::string& atlasDir);
	bool parseSprite(std::istream& atlas, std::string& line, sf::Sprite& sprite) const;
	bool parseOption(const std::string& line, std::string& name, std::string& value) const;
	bool parseVector2i(const std::string& value, sf::Vector2i& vector) const;
	
	std::vector<sf::Texture> mPages;
	std::unordered_map<std::string, std::string> mOptions;
	std::unordered_map<std::string, sf::Sprite> mSprites;
};

#endif /* KJC_TEXTUREATLAS_H */
