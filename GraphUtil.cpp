#include "GraphUtil.hpp"

sf::ConvexShape Line(const sf::Vector2f &Start, const sf::Vector2f &End, float Thickness, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::Vector2f Normal(Start.y - End.y, End.x - Start.x);
    float Length = Distance(sf::Vector2f(0.f, 0.f), Normal);
    if (Length != 0.f)
        Normal /= Length;

    Normal *= Thickness / 2;


    sf::ConvexShape Line;

    Line.SetPointCount(4);
    Line.SetPoint(0, Start - Normal);
    Line.SetPoint(1, End - Normal);
    Line.SetPoint(2, End + Normal);
    Line.SetPoint(3, Start + Normal);
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
