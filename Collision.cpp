#include "Collision.hpp"
#include "Math.hpp"
#include <utility>
#include <iostream>

using namespace std;

typedef pair<float, float> Projection;

Projection Project(const sf::Sprite &Shape, const sf::Vector2f &Axis)
{
    Projection P;
    P.first = P.second = DotProduct(Axis, Shape.GetTransform().TransformPoint(0.f, 0.f)); // initial projection

    for (int i = 1; i < 4; i++)
    {
        sf::Vector2f Point;
        switch (i)
        {
            case 1:
                Point = sf::Vector2f(Shape.GetLocalBounds().Width, 0.f);
                break;
            case 2:
                Point = sf::Vector2f(0.f, Shape.GetLocalBounds().Height);
                break;
            case 3:
                Point = sf::Vector2f(Shape.GetLocalBounds().Width, Shape.GetLocalBounds().Height);
                break;
        }

        float Proj = DotProduct(Axis, Shape.GetTransform().TransformPoint(Point));
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
    if (!A.GetGlobalBounds().Intersects(B.GetGlobalBounds())) // AABB collision
        return false;

    sf::Vector2f Axes[4];
    for (int i = 0; i < 4; i++) // find all axes that need testing
    {
        const sf::Sprite &Shape = (i / 2 == 0 ? A : B); // alternate rectangle
        const sf::Transform &T = Shape.GetTransform();

        sf::Vector2f P1 = T.TransformPoint(0.f, 0.f);
        sf::Vector2f P2 = T.TransformPoint(i % 2 == 0 ? sf::Vector2f(Shape.GetLocalBounds().Width, 0.f) : sf::Vector2f(0.f, Shape.GetLocalBounds().Height));
        cout << P1.x << ";" << P1.y << "  " << P2.x << ";" << P2.y << endl;

        sf::Vector2f Edge = P1 - P2; // alternate edge
        Axes[i] = Normalize(sf::Vector2f(-Edge.y, Edge.x)); // perpendicular vector

        cout << " " << Axes[i].x << ";" << Axes[i].y << endl;
    }

    for (int i = 0; i < 4; i++) // test all axes
    {
        Projection PA = Project(A, Axes[i]);
        Projection PB = Project(B, Axes[i]);

        cout << "." << PA.first << "-" << PA.second << "  " << PB.first << "-" << PB.second << endl;

        if (!ProjectionIntersect(PA, PB))
        {
            cout << "NO INTER" << endl;
            return false;
        }
    }

    cout << "INTER" << endl;
    cout << endl;

    return true; /// HOLY FUCKING FIX
}
