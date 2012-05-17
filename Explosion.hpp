#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <map>

using namespace std;

struct ExplosionTemplate
{
    string Name;
    string Res;
    string SoundRes;
    float Radius;
    float Time;
};

class Explosion
{
    public:
        friend class Aircraft;
        Explosion(const ExplosionTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos);
        ExplosionTemplate GetTemplate();

        sf::Vector2f GetPos();
        float GetRadius();

        bool Deadly();

        void Pause(bool Status);

        bool Step(float FT);
        void Draw(sf::RenderWindow &App);
    protected:
    private:
        const ExplosionTemplate &Template;

        sf::Sprite Shape;

        float TTL, Time;
        float Radius;

        sf::Sound Sound;
};

#endif // EXPLOSION_H
