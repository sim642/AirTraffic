#include "Runway.hpp"

Runway::Runway(RunwayTemplate NewTemplate, map<string, sf::Image> &Images, sf::Vector2f Pos, float Rot) : Template(NewTemplate)
{
    Shape.SetImage(Images[Template.Res]);
    Shape.SetOrigin(Template.Center);
    Shape.SetPosition(Pos);
    Shape.SetRotation(Rot);

    Radius = Template.Radius;
}

RunwayTemplate Runway::GetTemplate()
{
    return Template;
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

float Runway::GetAngle()
{
    return Shape.GetRotation();
}

void Runway::Draw(sf::RenderWindow &App)
{
    App.Draw(Shape);
}


