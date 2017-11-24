//
//  helpers.h
//
//  Created by Kevin Colley on 10/5/17.
//  Copyright © 2017 Kevin Colley. All rights reserved.
//

#ifndef KJC_HELPERS_H
#define KJC_HELPERS_H

#include <string>
#include <SFML/Graphics.hpp>
#include <cmath>

#define ABS(x) ({ \
	__typeof__(x) _x = (x); \
	_x < 0 ? -_x : _x; \
})

#define CMP(op, a, b) ({ \
	__typeof__(a) _a = (a); \
	__typeof__(b) _b = (b); \
	(_a op _b) ? _a : _b; \
})
#define MIN(a, b) CMP(<, a, b)
#define MAX(a, b) CMP(>, a, b)

#define CLAMP(x, lo, hi) MAX(lo, MIN(x, hi))

#define ARRSIZE(arr) (sizeof(arr) / sizeof(*(arr)))

#define RAD2DEG (180.0 / M_PI)


/**
 * Retrieve the filesystem path where resources are contained.
 * @returns Filesystem path to resources directory
 */
std::string resourcePath();

/**
 * Strip leading and trailing whitespace from a string and return the result as a new string.
 * @param str String from which surrounding whitespace should be stipped
 * @return New string containing the contents of @p str after removing surrounding whitespace
 */
std::string strip(const std::string& str);

/**
 * Return the X-coordinate of the right side of the sprite's bounds.
 * @param spr Sprite from which to retrieve position information
 * @return X-coordinate of the right side of @p spr's bounds in world-space
 */
float right(const sf::Sprite& spr);

/**
 * Return the Y-coordinate of the bottom side of the sprite's bounds.
 * @param spr Sprite from which to retrieve position information
 * @return Y-coordinate of the bottom side of @p spr's bounds in world-space
 */
float bottom(const sf::Sprite& spr);

/**
 * Return the X-coordinate of the center of the sprite's bounds.
 * @param spr Sprite from which to retrieve position information
 * @return X-coordinate of the center of @p spr's bounds in world-space
 */
float midX(const sf::Sprite& spr);

/**
 * Return the Y-coordinate of the center of the sprite's bounds.
 * @param spr Sprite from which to retrieve position information
 * @return Y-coordinate of the center of @p spr's bounds in world-space
 */
float midY(const sf::Sprite& spr);

/**
 * Return the point in the center of the sprite's bounds.
 * @param spr Sprite from which to retrieve position information
 * @return Vector to the center of the sprite's bounds
 */
sf::Vector2f getCenter(const sf::Sprite& spr);

/**
 * Return the actual size of the sprite after applying scale.
 * @param spr Sprite from which to retrieve size information
 * @return Vector representing the size of the sprite in pixels
 */
sf::Vector2f getSize(const sf::Sprite& spr);

/**
 * Return the actual width of the sprite after applying scale.
 * @param spr Sprite from which to retrieve size information
 * @return Width of the sprite in pixels
 */
float spriteWidth(const sf::Sprite& spr);

/**
 * Return the actual height of the sprite after applying scale.
 * @param spr Sprite from which to retrieve size information
 * @return Height of the sprite in pixels
 */
float spriteHeight(const sf::Sprite& spr);

/**
 * Set the sprite's actual size by changing its scale.
 * @param spr Sprite whose scale should be changed to make it be the given size
 * @param size Actual size to which @p spr should be scaled
 */
void setSize(sf::Sprite& spr, sf::Vector2f size);


/**
 * Calculates |v|**2, more efficiently than squaring the result of magnitude(v).
 */
template <typename T>
T sqrmagnitude(const sf::Vector2<T>& v) {
	return v.x*v.x + v.y*v.y;
}

/**
 * Calculates |v|.
 */
template <typename T>
T magnitude(const sf::Vector2<T>& v) {
	return sqrt(sqrmagnitude(v));
}

/**
 * Calculates the distance between a and b, squared.
 */
template <typename T>
T sqrdistance(const sf::Vector2<T>& a, const sf::Vector2<T> b) {
	T dx = a.x - b.x;
	T dy = a.y - b.y;
	
	return dx*dx + dy*dy;
}

/**
 * Calculates the distance between a and b.
 */
template <typename T>
T distance(const sf::Vector2<T>& a, const sf::Vector2<T> b) {
	return sqrt(sqrdistance(a, b));
}

/**
 * Calculates |v|**2, more efficiently than squaring the result of magnitude(v).
 */
template <typename T>
T sqrmagnitude(const sf::Vector3<T>& v) {
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

/**
 * Calculates |v|.
 */
template <typename T>
T magnitude(const sf::Vector3<T>& v) {
	return sqrt(sqrmagnitude(v));
}

/**
 * Calculates the distance between a and b, squared.
 */
template <typename T>
T sqrdistance(const sf::Vector3<T>& a, const sf::Vector3<T> b) {
	T dx = a.x - b.x;
	T dy = a.y - b.y;
	T dz = a.z - b.z;
	
	return dx*dx + dy*dy + dz*dz;
}

/**
 * Calculates the distance between a and b.
 */
template <typename T>
T distance(const sf::Vector3<T>& a, const sf::Vector3<T> b) {
	return sqrt(sqrdistance(a, b));
}

/**
 * Return the top-left point of the rectangle.
 */
template <typename T>
sf::Vector2<T> topLeft(const sf::Rect<T>& rect) {
	return {rect.left, rect.top};
}

/**
 * Return the top-right point of the rectangle.
 */
template <typename T>
sf::Vector2<T> topRight(const sf::Rect<T>& rect) {
	return {rect.left + rect.width, rect.top};
}

/**
 * Return the bottom-left point of the rectangle.
 */
template <typename T>
sf::Vector2<T> bottomLeft(const sf::Rect<T>& rect) {
	return {rect.left, rect.top + rect.height};
}

/**
 * Return the bottom-right point of the rectangle.
 */
template <typename T>
sf::Vector2<T> bottomRight(const sf::Rect<T>& rect) {
	return {rect.left + rect.width, rect.top + rect.height};
}

/**
 * Return the X-coordinate of the right edge of the rectangle.
 */
template <typename T>
T right(const sf::Rect<T>& rect) {
	return rect.left + rect.width;
}

/**
 * Return the Y-coordinate of the bottom edge of the rectangle.
 */
template <typename T>
T bottom(const sf::Rect<T>& rect) {
	return rect.top + rect.height;
}

/**
 * Return the X-coordinate of the center of the rectangle.
 */
template <typename T>
T midX(const sf::Rect<T>& rect) {
	return rect.left + rect.width / 2;
}

/**
 * Return the Y-coordinate of the center of the rectangle.
 */
template <typename T>
T midY(const sf::Rect<T>& rect) {
	return rect.top + rect.height / 2;
}

/**
 * Return the point in the center of the rectangle.
 */
template <typename T>
sf::Vector2<T> getCenter(const sf::Rect<T>& rect) {
	return {rect.left + rect.width / 2, rect.top + rect.height / 2};
}

/**
 * Return the size of the rectangle as a size vector.
 */
template <typename T>
sf::Vector2<T> getSize(const sf::Rect<T>& rect) {
	return {rect.width, rect.height};
}

/**
 * Sets the size of the rectangle given a size vector.
 */
template <typename T>
void setSize(sf::Rect<T>& rect, const sf::Vector2<T>& size) {
	rect.width = size.x;
	rect.height = size.y;
}

#endif /* KJC_HELPERS_H */
