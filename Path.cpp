#include "Path.hpp"

Path::Path() : Highlight(false)
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
    if (!(Points.size() > 0 &&
          pow(Points[Points.size() - 1].x - Point.x, 2) + pow(Points[Points.size() - 1].y - Point.y, 2) < pow(10, 2)))
    {
        AddPoint(Point);
        return true;
    }
    else
    {
        return false;
    }
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
    return Points[i];
}

void Path::Draw(sf::RenderWindow& App)
{
    if (NumPoints() > 1)
    {
        for (vector<sf::Vector2f>::iterator it = Points.begin() + 1; it != Points.end(); ++it)
        {
            App.Draw(sf::Shape::Line(*(it - 1), *it, 2.f, Highlight ? sf::Color::Yellow : sf::Color::Blue));
        }
    }
}


