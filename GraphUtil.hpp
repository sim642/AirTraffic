#ifndef GRAPHUTIL_H
#define GRAPHUTIL_H

#include <SFML/Graphics.hpp>
#include <vector>

sf::ConvexShape Line(const sf::Vector2f &Start, const sf::Vector2f &End, float Thickness, const sf::Color &Color, float Outline = 0.f, const sf::Color &OutlineColor = sf::Color(0, 0, 0));
sf::CircleShape Circle(const sf::Vector2f &Center, float Radius, const sf::Color &Color, float Outline = 0.f, const sf::Color &OutlineColor = sf::Color(0, 0, 0));
sf::RectangleShape Rectangle(float Left, float Top, float Width, float Height, const sf::Color &Color, float Outline = 0.f, const sf::Color &OutlineColor = sf::Color(0, 0, 0));

sf::ConvexShape ConvexHull(std::vector<sf::Vector2f> Points);

sf::Texture PerlinNoise();

#endif // GRAPHUTIL_H
