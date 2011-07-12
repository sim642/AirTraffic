#include "Runway.hpp"

Runway::Runway(vector<sf::Image> &Images, sf::Vector2f Pos, float Rot)
{
    const sf::Image &Image = Images[rand() % Images.size()];
    Shape.SetImage(Image);
    Shape.SetCenter(Image.GetHeight() / 2, Image.GetHeight() / 2);
    Shape.SetPosition(Pos);
    Shape.SetRotation(Rot);

    Radius = Image.GetHeight() / 2.f * 1.5f;
}

bool Runway::OnMe(sf::Vector2f Pos)
{
    const sf::Vector2f &Me = Shape.GetPosition();
    return pow(Pos.x - Me.x, 2) + pow(Pos.y - Me.y, 2) < pow(Radius, 2);
}

sf::Vector2f Runway::GetPos()
{
    return Shape.GetPosition();
}

void Runway::Draw(sf::RenderWindow &App)
{
    App.Draw(Shape);
}


