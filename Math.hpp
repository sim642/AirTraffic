#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <SFML/System/Vector2.hpp>

using namespace std;

const float PI = 3.14159265f;

template<typename T>
inline T Map(T X, T InMin, T InMax, T OutMin, T OutMax)
{
    return (X - InMin) * (OutMax - OutMin) / (InMax - InMin) + OutMin;
}

inline float DegToRad(float Deg)
{
    return Deg * PI / 180.f;
}

inline float RadToDeg(float Rad)
{
    return Rad * 180.f / PI;
}

inline float AngleFix(float Angle)
{
    return 360.f - Angle;
}

inline float AngleDiff(float Angle1, float Angle2)
{
    float Diff = Angle2 - Angle1;
    while (Diff < -180.f)
        Diff += 360.f;

    while (Diff > 180.f)
        Diff -= 360.f;

    return Diff;
}

inline float Random(float Min, float Max)
{
    return (Max - Min) * (rand() / static_cast<float>(RAND_MAX)) + Min;
}

inline int Random(int Min, int Max)
{
    return rand() % (Max - Min + 1) + Min;
}

inline bool Chance(float Cha)
{
    return (rand() / static_cast<float>(RAND_MAX)) < Cha;
}

inline float Distance(sf::Vector2f P1, sf::Vector2f P2)
{
    return sqrt(pow(P2.x - P1.x, 2) + pow(P2.y - P1.y, 2));
}

inline bool InRange(sf::Vector2f P1, sf::Vector2f P2, float Dist)
{
    return pow(P2.x - P1.x, 2) + pow(P2.y - P1.y, 2) < pow(Dist, 2);
}

inline float Magnitude(sf::Vector2f V)
{
    return Distance(sf::Vector2f(0.f, 0.f), V);
}

inline float Angle(sf::Vector2f V)
{
    return RadToDeg(atan2(-V.y, -V.x));
}

inline sf::Vector2f Normalize(sf::Vector2f V)
{
    return V / Magnitude(V);
}

inline sf::Vector2f Scale(sf::Vector2f V, float S)
{
    return Normalize(V) * S;
}

inline sf::Vector2f Rotate(sf::Vector2f V, float Deg)
{
    float Rad = DegToRad(Deg);
    sf::Vector2f New;
    New.x = V.x * cos(Rad) - V.y * sin(Rad);
    New.y = V.x * sin(Rad) + V.y * cos(Rad);
    return New;
}

inline sf::Vector2f PolarToRect(sf::Vector2f Polar)
{
    sf::Vector2f Rect(Polar.x, 0.f);
    return Rotate(Rect, Polar.y);
}


#endif // MATH_H
