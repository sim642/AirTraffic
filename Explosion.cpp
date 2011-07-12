#include "Explosion.hpp"

Explosion::Explosion(vector<sf::Image> &Images, sf::Vector2f Pos) : TTL(3.f), Time(0.f)
{
    const sf::Image &Image = Images[rand() % Images.size()];
    Shape.SetImage(Image);
    Shape.SetCenter(Image.GetWidth() / 2, Image.GetHeight() / 2);
    Shape.SetPosition(Pos);

    Radius = (Image.GetWidth() + Image.GetHeight()) / 2.f;
}

sf::Vector2f Explosion::GetPos()
{
    return Shape.GetPosition();
}

bool Explosion::Deadly()
{
    return (Time / TTL) < 0.60f;
}

bool Explosion::Step(float FT)
{
    Time += FT;
    return Time > TTL;
}

void Explosion::Draw(sf::RenderWindow &App)
{
    //Shape.SetColor(sf::Color(255, 255, 255, 255 - (Time / TTL) * 255));
    Shape.SetColor(sf::Color(255, 255, 255, 255 - pow(Time / TTL, 2) * 255));
    App.Draw(Shape);
}
