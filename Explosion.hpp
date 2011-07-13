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
        Explosion(ExplosionTemplate, map<string, sf::Image>&, sf::Vector2f);
        ExplosionTemplate GetTemplate();

        sf::Vector2f GetPos();

        bool Deadly();

        bool Step(float);
        void Draw(sf::RenderWindow&);
    protected:
    private:
        const ExplosionTemplate Template;

        sf::Sprite Shape;

        float TTL, Time;
        float Radius;
};

#endif // EXPLOSION_H
