#include "GraphUtil.hpp"

sf::RectangleShape Line(const sf::Vector2f &Start, const sf::Vector2f &End, float Thickness, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    //doesn't have rounded ends
    sf::RectangleShape Line;
    Line.SetPosition(Start);
    Line.SetSize(sf::Vector2f(Distance(Start, End), Thickness));
    Line.SetRotation(RadToDeg(atan2(End.y - Start.y, End.x - Start.x)));
    Line.SetFillColor(Color);
    Line.SetOutlineThickness(Outline);
    Line.SetOutlineColor(OutlineColor);
    return Line;
}

sf::CircleShape Circle(const sf::Vector2f &Center, float Radius, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::CircleShape Circle;
    Circle.SetOrigin(Radius, Radius);
    Circle.SetPosition(Center);
    Circle.SetRadius(Radius);
    Circle.SetFillColor(Color);
    Circle.SetOutlineThickness(Outline);
    Circle.SetOutlineColor(OutlineColor);
    return Circle;
}

sf::RectangleShape Rectangle(float Left, float Top, float Width, float Height, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::RectangleShape Rectangle;
    Rectangle.SetPosition(Left, Top);
    Rectangle.SetSize(sf::Vector2f(Width, Height));
    Rectangle.SetFillColor(Color);
    Rectangle.SetOutlineThickness(Outline);
    Rectangle.SetOutlineColor(OutlineColor);
    return Rectangle;
}
