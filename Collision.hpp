#ifndef COLLISION_H
#define COLLISION_H

#include <SFML/Graphics.hpp>

sf::RectangleShape SpriteToShape(const sf::Sprite &Sprite);

bool CollidingShapes(const sf::Shape &A, const sf::Shape &B);
bool CollidingShapes(const sf::Sprite &A, const sf::Shape &B);
bool CollidingShapes(const sf::Shape &A, const sf::Sprite &B);
bool CollidingShapes(const sf::Sprite &A, const sf::Sprite &B);

#endif // COLLISION_H
