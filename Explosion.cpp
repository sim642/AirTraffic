#include "Explosion.hpp"
#include "Math.hpp"

Explosion::Explosion(const ExplosionTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float NewTime) : Template(NewTemplate), Time(NewTime)
{
    const sf::Texture &Texture = Textures[Template.Res];
    Shape.setTexture(Texture);
    Shape.setOrigin(sf::Vector2f(Texture.getSize()) / 2.f);
    Shape.setPosition(Pos);

    Sound.setBuffer(Sounds[Template.SoundRes]);
    Sound.setAttenuation(0.01f);
    Sound.setPosition(Pos.x, Pos.y, 0.f);
    Sound.setPlayingOffset(sf::seconds(Time));
    Sound.play();

    Radius = Template.Radius;
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
    Time += FT;
    return Time > TTL;
}

void Explosion::Draw(sf::RenderWindow &App)
{
    //Shape.SetColor(sf::Color(255, 255, 255, 255 - (Time / TTL) * 255));
    Shape.setColor(sf::Color(255, 255, 255, 255 - pow(Time / TTL, 2) * 255));
    App.draw(Shape);
}
