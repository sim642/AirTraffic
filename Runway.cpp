#include "Runway.hpp"
#include "Math.hpp"
#include "Collision.hpp"
#include "GraphUtil.hpp"

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

    for (int i = 0; i < 4; i++)
    {
        sf::Vector2f Point;
        switch (i)
        {
            case 0:
                Point = sf::Vector2f(0.f, 0.f);
                break;
            case 1:
                Point = sf::Vector2f(Shape.GetLocalBounds().Width, 0.f);
                break;
            case 2:
                Point = sf::Vector2f(0.f, Shape.GetLocalBounds().Height);
                break;
            case 3:
                Point = sf::Vector2f(Shape.GetLocalBounds().Width, Shape.GetLocalBounds().Height);
                break;
        }

        App.Draw(Circle(Shape.GetTransform().TransformPoint(Point), 2.f, i == 0 ? sf::Color::Blue : sf::Color::Red));
    }
}


