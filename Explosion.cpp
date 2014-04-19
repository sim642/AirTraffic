#include "Explosion.hpp"
#include "Math.hpp"

Explosion::Explosion(const ExplosionTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float NewTime) : Template(NewTemplate), Time(NewTime)
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

    Sound.setBuffer(Sounds[Template.SoundRes]);
    Sound.setAttenuation(0.01f);
    Sound.setPosition(Pos.x, Pos.y, 0.f);
    Sound.setPlayingOffset(sf::seconds(Time));
    Sound.play();

    Radius = Template.Radius;
    if (Template.Time == 0.f)
		TTL = Shape.getFrameCount() / Shape.getFrameRate();
	else
		TTL = Template.Time;
}

ExplosionTemplate Explosion::GetTemplate()
{
    return Template;
}

sf::Vector2f Explosion::GetPos()
{
    return Shape.getPosition();
}

float Explosion::GetRadius()
{
    return Radius;
}

bool Explosion::Deadly()
{
    return (Time / TTL) < 0.60f;
}

float Explosion::GetTime()
{
    return Time;
}

void Explosion::Pause(bool Status)
{
    if (Status)
    {
        Sound.pause();
    }
    else
    {
        if (Sound.getStatus() == sf::Sound::Paused)
        {
            Sound.play();
        }
    }
}

bool Explosion::Step(float FT)
{
	Shape.update(FT);
    Time += FT;
    return Time > TTL;
}

void Explosion::Draw(sf::RenderWindow &App)
{
    //Shape.SetColor(sf::Color(255, 255, 255, 255 - (Time / TTL) * 255));
    if (Template.Time != 0.f)
		Shape.setColor(sf::Color(255, 255, 255, 255 - pow(Time / TTL, 2) * 255));
    App.draw(Shape);
}
