#include "Runway.hpp"
#include "Math.hpp"
#include "Collision.hpp"

Runway::Runway(const RunwayTemplate &NewTemplate, map<string, sf::Texture> &Texture, sf::Vector2f Pos, float Rot) : Template(NewTemplate)
{
    Shape.SetTexture(Texture[Template.Res]);
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
    return InRange(Me, Pos, Radius);
}

bool Runway::Colliding(const Runway &Other)
{
    return CollidingSprites(Shape, Other.Shape);
}

sf::Vector2f Runway::GetPos()
{
    return Shape.GetPosition();
}

float Runway::GetAngle()
{
    return Shape.GetRotation();
}

float Runway::GetLength()
{
    return Template.Length;
}

void Runway::Draw(sf::RenderWindow &App)
{
    App.Draw(Shape);
}


