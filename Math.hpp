#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <wrench/Math.hpp>

using namespace std;

const float PI = 3.14159265f;

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


#endif // MATH_H
