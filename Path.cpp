#include "Path.hpp"
#include <algorithm>
#include "Math.hpp"
#include "GraphUtil.hpp"

Path::Path() : Highlight(false), interp(5)
{
    //ctor
}

int Path::AddPoint(sf::Vector2f Point)
{
    Points.push_back(Point);

    return NumPoints() - 1;
}

bool Path::TryAddPoint(sf::Vector2f Point)
{
    bool Added = false;
    if (Points.size() < 1)
    {
        Points.push_back(Point);
        Added = true;
    }
    else if (!InRange(Point, Points.back(), 20.f))
    {
        if (Points.size() < 2)
        {
            double step = 1.0 / interp;
            double t = step;
            sf::Vector2f last = Points.back();
            for (unsigned int i = 1; i < interp; i++, t += step)
            {
                Points.push_back(last + static_cast<float>(t) * (Point - last));
            }
        }
        else
        {
            static double shit = 0.5;

            sf::Vector2f p0 = Points.at(max<int>(0, Points.size() - 2 - (interp - 1)));
            sf::Vector2f p1 = Points.back();
            sf::Vector2f p2 = Point;
            sf::Vector2f m0, m1, m2;

            if (Points.size() <= (3 + 2 * (interp - 1)))
            {
                m0 = sf::Vector2f(0.f, 0.f);
            }
            else
            {
                m0.x = tangent(Points.at(Points.size() - 3 - 2 * (interp - 1)).x, p0.x, p1.x, shit);
                m0.y = tangent(Points.at(Points.size() - 3 - 2 * (interp - 1)).y, p0.y, p1.y, shit);
            }
            m1.x = tangent(p0.x, p1.x, p2.x, shit);
            m1.y = tangent(p0.y, p1.y, p2.y, shit);
            m2 = sf::Vector2f(0.f, 0.f);

            double step = 1.0 / interp;
            double t = step;
            for (unsigned int i = 0; i < interp; i++, t += step)
            {
                int j = Points.size() - 2 - (interp - 1) + 1 + i; // not perfect fix
                if (j < 0)
                    continue;
                sf::Vector2f &p = Points.at(j);
                p.x = f(p0.x, p1.x, m0.x, m1.x, t);
                p.y = f(p0.y, p1.y, m0.y, m1.y, t);
            }
            t = step;
            for (unsigned int i = 1; i < interp; i++, t += step)
            {
                sf::Vector2f p;
                p.x = f(p1.x, p2.x, m1.x, m2.x, t);
                p.y = f(p1.y, p2.y, m1.y, m2.y, t);
                Points.push_back(p);
            }
        }
        Points.push_back(Point);
        Added = true;
    }
    return Added;
}

float Path::EndAngle()
{
    if (Points.size() < 2)
        throw;

    sf::Vector2f &Point0 = *Points.rbegin();
    sf::Vector2f &Point1 = *(Points.rbegin() + 1);

    float Angle = RadToDeg(atan2(Point0.y - Point1.y, Point0.x - Point1.x));
    if (Angle < 0.f)
        Angle += 360.f;
    return Angle;
}

void Path::RemovePoint(int i)
{
    Points.erase(Points.begin() + i);
}

void Path::Clear()
{
    Points.clear();
}

int Path::NumPoints()
{
    return Points.size();
}

sf::Vector2f Path::operator[](int i)
{
    return Points.at(i);
}

void Path::Draw(sf::RenderWindow& App)
{
    if (NumPoints() > 1)
    {
        for (vector<sf::Vector2f>::iterator it = Points.begin() + 1; it != Points.end(); ++it)
        {
            App.draw(Line(*(it - 1), *it, 2.f, Highlight ? sf::Color::Yellow : sf::Color::Blue));
        }
    }
}

double Path::tangent(double p0, double p1, double p2, double shit)
{
    return ((p2 - p1) / (2 * (shit + shit))) + ((p1 - p0) / (2 * (shit + shit)));
}

double Path::f(double p0, double p1, double m0, double m1, double t)
{
    return (2 * pow(t,3) - 3 * pow(t,2) + 1) * p0 + (pow(t,3) - 2 * pow(t,2) + t) * m0 + (-2 * pow(t,3) + 3 * pow(t,2)) * p1 + (pow(t,3) - pow(t,2)) * m1;
}
