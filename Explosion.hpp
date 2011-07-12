#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Math.hpp"

using namespace std;

class Explosion
{
    public:
        friend class Aircraft;
        Explosion(vector<sf::Image>&, sf::Vector2f);

        sf::Vector2f GetPos();

        bool Deadly();

        bool Step(float);
        void Draw(sf::RenderWindow&);
    protected:
    private:
        sf::Sprite Shape;

        float TTL, Time;
        float Radius;
};

#endif // EXPLOSION_H
