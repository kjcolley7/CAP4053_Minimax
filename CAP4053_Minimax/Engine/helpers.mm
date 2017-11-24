//
//  helpers.mm
//
//  Created by Kevin Colley on 10/5/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#include "helpers.h"
#import <Foundation/Foundation.h>
#include <cctype>


std::string resourcePath() {
	std::string ret;
	
	@autoreleasepool {
		NSBundle* bundle = [NSBundle mainBundle];
		if(bundle) {
			NSString* path = [bundle resourcePath];
			ret = [path UTF8String] + std::string("/");
		}
	}
	
	return ret;
}

std::string strip(const std::string& str) {
	size_t i = 0;
	
	// Skip leading spaces
	while(std::isspace(str[i])) {
		++i;
	}
	size_t start = i;
	
	// Skip trailing spaces
	i = str.size();
	while(std::isspace(str[i-1])) {
		--i;
	}
	
	// Chop off spaces at start and end
	return str.substr(start, i - start);
}

float right(const sf::Sprite& spr) {
	return spr.getPosition().x + spr.getScale().x * spr.getTextureRect().width;
}

float bottom(const sf::Sprite& spr) {
	return spr.getPosition().y + spr.getScale().y * spr.getTextureRect().height;
}

float midX(const sf::Sprite& spr) {
	return spr.getPosition().y + spriteWidth(spr) / 2.0f;
}

float midY(const sf::Sprite& spr) {
	return spr.getPosition().x + spriteHeight(spr) / 2.0f;
}

sf::Vector2f getCenter(const sf::Sprite& spr) {
	return spr.getPosition() + getSize(spr) / 2.0f;
}

sf::Vector2f getSize(const sf::Sprite& spr) {
	sf::Vector2f ret{getSize(spr.getTextureRect())};
	sf::Vector2f scale{spr.getScale()};
	ret.x *= scale.x;
	ret.y *= scale.y;
	
	return ret;
}

float spriteWidth(const sf::Sprite& spr) {
	return spr.getTextureRect().width * spr.getScale().x;
}

float spriteHeight(const sf::Sprite& spr) {
	return spr.getTextureRect().height * spr.getScale().y;
}

void setSize(sf::Sprite& spr, sf::Vector2f size) {
	sf::Vector2f texSize{getSize(spr.getTextureRect())};
	size.x /= texSize.x;
	size.y /= texSize.y;
	
	spr.setScale(size);
}
