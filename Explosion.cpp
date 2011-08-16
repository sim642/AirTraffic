#include "Explosion.hpp"

Explosion::Explosion(ExplosionTemplate NewTemplate, map<string, sf::Image> &Images, sf::Vector2f Pos) : Template(NewTemplate), Time(0.f)
{
    const sf::Image &Image = Images[Template.Res];
    Shape.SetImage(Image);
    Shape.SetOrigin(Image.GetWidth() / 2, Image.GetHeight() / 2);
    Shape.SetPosition(Pos);

    Radius = Template.Radius;
    TTL = Template.Time;
}

ExplosionTemplate Explosion::GetTemplate()
{
    return Template;
}

sf::Vector2f Explosion::GetPos()
{
    return Shape.GetPosition();
}

float Explosion::GetRadius()
{
    return Radius;
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
