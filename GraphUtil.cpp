#include "GraphUtil.hpp"
#include "Math.hpp"

sf::ConvexShape Line(const sf::Vector2f &Start, const sf::Vector2f &End, float Thickness, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::Vector2f Normal(Start.y - End.y, End.x - Start.x);
    float Length = Distance(sf::Vector2f(0.f, 0.f), Normal);
    if (Length != 0.f)
        Normal /= Length;

    Normal *= Thickness / 2;


    sf::ConvexShape Line;

    Line.setPointCount(4);
    Line.setPoint(0, Start - Normal);
    Line.setPoint(1, End - Normal);
    Line.setPoint(2, End + Normal);
    Line.setPoint(3, Start + Normal);
    Line.setFillColor(Color);
    Line.setOutlineThickness(Outline);
    Line.setOutlineColor(OutlineColor);

    return Line;
}

sf::CircleShape Circle(const sf::Vector2f &Center, float Radius, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::CircleShape Circle;
    Circle.setOrigin(Radius, Radius);
    Circle.setPosition(Center);
    Circle.setRadius(Radius);
    Circle.setFillColor(Color);
    Circle.setOutlineThickness(Outline);
    Circle.setOutlineColor(OutlineColor);
    return Circle;
}

sf::RectangleShape Rectangle(float Left, float Top, float Width, float Height, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::RectangleShape Rectangle;
    Rectangle.setPosition(Left, Top);
    Rectangle.setSize(sf::Vector2f(Width, Height));
    Rectangle.setFillColor(Color);
    Rectangle.setOutlineThickness(Outline);
    Rectangle.setOutlineColor(OutlineColor);
    return Rectangle;
}
