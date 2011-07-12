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


