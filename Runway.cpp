#include "Runway.hpp"
#include "Math.hpp"
#include "Collision.hpp"

Runway::Runway(const RunwayTemplate &NewTemplate, map<string, sf::Texture> &Texture, sf::Vector2f Pos, float Rot) : Template(NewTemplate)
{
    Shape.setTexture(Texture[Template.Res]);
    Shape.setOrigin(Template.Center);
    Shape.setPosition(Pos);
    Shape.setRotation(Rot);

    Radius = Template.Radius;
}

RunwayTemplate Runway::GetTemplate()
{
    return Template;
}

bool Runway::OnMe(sf::Vector2f Pos)
{
    const sf::Vector2f &Me = Shape.getPosition();
    return InRange(Me, Pos, Radius);
}

bool Runway::Colliding(const Runway &Other)
{
    return CollidingSprites(Shape, Other.Shape);
}

sf::Vector2f Runway::GetPos()
{
    return Shape.getPosition();
}

float Runway::GetAngle()
{
    return Shape.getRotation();
}

float Runway::GetLength()
{
    return Template.Length;
}

sf::Sprite Runway::GetShape()
{
    return Shape;
}

void Runway::Draw(sf::RenderWindow &App)
{
    App.draw(Shape);
}


