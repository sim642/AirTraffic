#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <map>
#include "AnimSprite.hpp"

using namespace std;

struct ExplosionTemplate
{
    string Name;
    string Res;
    sf::Vector2i FrameSize;
    float FrameRate;
    string SoundRes;
    float Radius;
    float Time;
};

class Explosion
{
    public:
        friend class Aircraft;
        Explosion(const ExplosionTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float NewTime = 0.f);
        ExplosionTemplate GetTemplate();

        sf::Vector2f GetPos();
        float GetRadius();

        bool Deadly();
        float GetTime();

        void Pause(bool Status);

        bool Step(float FT);
        void Draw(sf::RenderWindow &App);
    protected:
    private:
        const ExplosionTemplate &Template;

        AnimSprite Shape;

        float TTL, Time;
        float Radius;

        sf::Sound Sound;
};

#endif // EXPLOSION_H
