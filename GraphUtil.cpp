#include "GraphUtil.hpp"
#include "Math.hpp"
#include <algorithm>

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

class BottomLeftCompare
{
public:
    bool operator() (const sf::Vector2f &Lhs, const sf::Vector2f &Rhs) const
    {
        if (Lhs.y == Rhs.y)
            return Lhs.x < Rhs.x;
        else
            return Lhs.y > Rhs.y;
    }
} BottomLeftComp;

class AngleCompare
{
public:
    AngleCompare(const sf::Vector2f &NewP) : P(NewP)
    {

    }

    bool operator() (const sf::Vector2f &Lhs, const sf::Vector2f &Rhs) const
    {
        return (-DotProduct(sf::Vector2f(1.f, 0.f), Lhs - P) / Magnitude(Lhs - P)) < (-DotProduct(sf::Vector2f(1.f, 0.f), Rhs - P) / Magnitude(Rhs - P));
    }
private:
    sf::Vector2f P;
};

float CCW(const sf::Vector2f &p1, const sf::Vector2f &p2, const sf::Vector2f &p3)
{
    return CrossProduct(p2 - p1, p3 - p1);
}

sf::ConvexShape ConvexHull(vector<sf::Vector2f> Points)
{
    iter_swap(Points.begin(), min_element(Points.begin(), Points.end(), BottomLeftComp));

    AngleCompare AngleComp(Points.front());
    sort(Points.begin() + 1, Points.end(), AngleComp);

    int m = 0;
    for (int i = 1; i < Points.size(); i++)
    {
        while (CCW(Points[m == 0 ? Points.size() - 1 : m - 1], Points[m], Points[i]) >= 0.f)
        {
            if (m > 0)
                m--;
            else if (i == Points.size() - 1)
                break;
            else
                i++;
        }

        m++;
        swap(Points[m], Points[i]);
    }

    sf::ConvexShape Shape(m + 1);
    for (int i = 0; i < Shape.getPointCount(); i++)
        Shape.setPoint(i, Points[i]);

    return Shape;
}
