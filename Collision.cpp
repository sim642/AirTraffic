#include "Collision.hpp"
#include "Math.hpp"
#include <utility>

using namespace std;

sf::RectangleShape SpriteToShape(const sf::Sprite &Sprite)
{
    sf::RectangleShape Shape(sf::Vector2f(Sprite.getLocalBounds().width, Sprite.getLocalBounds().height));
    Shape.setOrigin(Sprite.getOrigin());
    Shape.setPosition(Sprite.getPosition());
    Shape.setRotation(Sprite.getRotation());
    Shape.setScale(Sprite.getScale());
    return Shape;
}

typedef pair<float, float> Projection;

Projection Project(const sf::Shape &Shape, const sf::Vector2f &Axis)
{
    Projection P;

    for (int i = 0; i < Shape.getPointCount(); i++)
    {
        sf::Vector2f Point = Shape.getPoint(i);

        float Proj = DotProduct(Axis, Shape.getTransform().transformPoint(Point));
        if (i == 0 || Proj < P.first)
            P.first = Proj;
        if (i == 0 || Proj > P.second)
            P.second = Proj;
    }

    return P;
}

bool ProjectionIntersect(const Projection &A, const Projection &B)
{
    return (B.first > A.first && B.first < A.second) || (B.second > A.first && B.second < A.second) || (A.first > B.first && A.first < B.second) || (A.second > B.first && A.second < B.second);
}

bool CollidingShapes(const sf::Shape &A, const sf::Shape &B)
{
    if (!A.getGlobalBounds().intersects(B.getGlobalBounds()))
        return false;

    vector<sf::Vector2f> Axes;
    for (int i = 1; i <= A.getPointCount(); i++)
        Axes.push_back(Normalize(Perpendicular(A.getPoint(i % A.getPointCount()) - A.getPoint(i - 1))));

    for (int i = 1; i <= B.getPointCount(); i++)
        Axes.push_back(Normalize(Perpendicular(B.getPoint(i % B.getPointCount()) - B.getPoint(i - 1))));

    for (vector<sf::Vector2f>::iterator it = Axes.begin(); it != Axes.end(); ++it)
    {
        Projection PA = Project(A, *it);
        Projection PB = Project(B, *it);

        if (!ProjectionIntersect(PA, PB))
            return false;
    }

    return true;
}

bool CollidingShapes(const sf::Sprite &A, const sf::Shape &B)
{
    return CollidingShapes(SpriteToShape(A), B);
}

bool CollidingShapes(const sf::Shape &A, const sf::Sprite &B)
{
    return CollidingShapes(A, SpriteToShape(B));
}

bool CollidingShapes(const sf::Sprite &A, const sf::Sprite &B)
{
    return CollidingShapes(SpriteToShape(A), SpriteToShape(B));
}
