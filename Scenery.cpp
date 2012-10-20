#include "Scenery.hpp"
#include "Collision.hpp"

Scenery::Scenery(const SceneryTemplate &NewTemplate, map<string, sf::Texture> &Textures, sf::Vector2f Pos, float Rot) : Template(NewTemplate)
{
    const sf::Texture &Texture = Textures[Template.Res];
    if (Template.FrameSize.x >= 0 || Template.FrameSize.y >= 0)
    {
        Shape = AnimSprite(Texture, Template.FrameSize, Template.FrameRate);
        Shape.SetOrigin(Template.FrameSize.x / 2.f, Template.FrameSize.y / 2.f);
    }
    else
    {
        Shape = AnimSprite(Texture, sf::Vector2i(Texture.GetWidth(), Texture.GetHeight()), 0.f);
        Shape.SetOrigin(Texture.GetWidth() / 2, Texture.GetWidth() / 2);
    }
    Shape.SetPosition(Pos);
    Shape.SetRotation(Rot);
    Shape.SetFrame(rand() % Shape.GetFrameCount());
}

SceneryTemplate Scenery::GetTemplate()
{
    return Template;
}

bool Scenery::Colliding(const Scenery &Other)
{
    return CollidingSprites(Shape, Other.Shape);
}

bool Scenery::Colliding(const Runway &Other)
{
    return CollidingSprites(Shape, Other.Shape);
}

sf::Vector2f Scenery::GetPos()
{
    return Shape.GetPosition();
}

float Scenery::GetAngle()
{
    return Shape.GetRotation();
}

void Scenery::Step(float FT)
{
    Shape.Update(FT);
}

void Scenery::Draw(sf::RenderWindow &App)
{
    App.Draw(Shape);
}
