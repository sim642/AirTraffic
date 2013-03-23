#include "Collision.hpp"
#include "Math.hpp"
#include <utility>

using namespace std;

typedef pair<float, float> Projection;

Projection Project(const sf::Sprite &Shape, const sf::Vector2f &Axis)
{
    Projection P;
    P.first = P.second = DotProduct(Axis, Shape.getTransform().transformPoint(0.f, 0.f)); // initial projection

    for (int i = 1; i < 4; i++)
    {
        sf::Vector2f Point;
        switch (i)
        {
            case 1:
                Point = sf::Vector2f(Shape.getLocalBounds().width, 0.f);
                break;
            case 2:
                Point = sf::Vector2f(0.f, Shape.getLocalBounds().height);
                break;
            case 3:
                Point = sf::Vector2f(Shape.getLocalBounds().width, Shape.getLocalBounds().height);
                break;
        }

        float Proj = DotProduct(Axis, Shape.getTransform().transformPoint(Point));
        if (Proj < P.first)
            P.first = Proj;
        else if (Proj > P.second)
            P.second = Proj;
    }

    return P;
}

bool ProjectionIntersect(const Projection &A, const Projection &B)
{
    return (B.first > A.first && B.first < A.second) || (B.second > A.first && B.second < A.second) || (A.first > B.first && A.first < B.second) || (A.second > B.first && A.second < B.second);
}

bool CollidingSprites(const sf::Sprite &A, const sf::Sprite &B)
{
    if (!A.getGlobalBounds().intersects(B.getGlobalBounds())) // AABB collision
        return false;

    sf::Vector2f Axes[4];
    for (int i = 0; i < 4; i++) // find all axes that need testing
    {
        const sf::Sprite &Shape = (i / 2 == 0 ? A : B); // alternate rectangle
        const sf::Transform &T = Shape.getTransform();

        sf::Vector2f P1 = T.transformPoint(0.f, 0.f);
        sf::Vector2f P2 = T.transformPoint(i % 2 == 0 ? sf::Vector2f(Shape.getLocalBounds().width, 0.f) : sf::Vector2f(0.f, Shape.getLocalBounds().height));

        sf::Vector2f Edge = P1 - P2; // alternate edge
        Axes[i] = Normalize(sf::Vector2f(-Edge.y, Edge.x)); // perpendicular vector
    }

    for (int i = 0; i < 4; i++) // test all axes
    {
        Projection PA = Project(A, Axes[i]);
        Projection PB = Project(B, Axes[i]);

        if (!ProjectionIntersect(PA, PB))
            return false;
    }

    return true;
}
