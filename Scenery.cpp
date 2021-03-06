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
    Shape.setFrame(rand() % Shape.getFrameCount());
}

SceneryTemplate Scenery::GetTemplate()
{
    return Template;
}

bool Scenery::Colliding(const Scenery &Other)
{
    return CollidingShapes(Shape, Other.Shape);
}

bool Scenery::Colliding(const Runway &Other)
{
    return CollidingShapes(Shape, Other.Shape);
}

sf::Vector2f Scenery::GetPos()
{
    return Shape.getPosition();
}

float Scenery::GetAngle()
{
    return Shape.getRotation();
}

const AnimSprite& Scenery::GetShape()
{
    return Shape;
}

void Scenery::Step(float FT)
{
    Shape.update(FT);
}

void Scenery::Draw(sf::RenderWindow &App)
{
    Shape.setColor(sf::Color::White);
    App.draw(Shape);
}

void Scenery::DrawShadow(sf::RenderWindow &App)
{
    Shape.setColor(sf::Color(0, 0, 0, 127));

    float Scale = Template.Shadow;

    sf::Transform Transform;
    Transform.scale(sf::Vector2f(Scale, Scale), sf::Vector2f(App.getSize().x / 2, App.getSize().y * 0.8f));
    App.draw(Shape, Transform);
}
