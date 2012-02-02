#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Math.hpp"

using namespace std;

struct ExplosionTemplate
{
    string Name;
    string Res;
    float Radius;
    float Time;
};

class Explosion
{
    public:
        friend class Aircraft;
        Explosion(ExplosionTemplate NewTemplate, map<string, sf::Texture> &Textures, sf::Vector2f Pos);
        ExplosionTemplate GetTemplate();

        sf::Vector2f GetPos();
        float GetRadius();

        bool Deadly();

        bool Step(float FT);
        void Draw(sf::RenderWindow &App);
    protected:
    private:
        const ExplosionTemplate Template;

        sf::Sprite Shape;

        float TTL, Time;
        float Radius;
};

#endif // EXPLOSION_H
