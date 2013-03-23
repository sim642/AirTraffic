#include "Scenery.hpp"
#include "Collision.hpp"

Scenery::Scenery(const SceneryTemplate &NewTemplate, map<string, sf::Texture> &Textures, sf::Vector2f Pos, float Rot) : Template(NewTemplate)
{
    const sf::Texture &Texture = Textures[Template.Res];
    if (Template.FrameSize.x >= 0 || Template.FrameSize.y >= 0)
    {
        Shape = AnimSprite(Texture, Template.FrameSize, Template.FrameRate);
        Shape.setOrigin(Template.FrameSize.x / 2.f, Template.FrameSize.y / 2.f);
    }
    else
    {
        Shape = AnimSprite(Texture, sf::Vector2i(Texture.getSize()), 0.f);
        Shape.setOrigin(sf::Vector2f(Texture.getSize()) / 2.f);
    }
    Shape.setPosition(Pos);
    Shape.setRotation(Rot);
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
    return Shape.getPosition();
}

float Scenery::GetAngle()
{
    return Shape.getRotation();
}

void Scenery::Step(float FT)
{
    Shape.Update(FT);
}

void Scenery::Draw(sf::RenderWindow &App)
{
    App.draw(Shape);
}
