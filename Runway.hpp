#ifndef RUNWAY_H
#define RUNWAY_H

#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;

class Runway
{
    public:
        Runway(vector<sf::Image>&, sf::Vector2f, float);

        bool OnMe(sf::Vector2f);

        sf::Vector2f GetPos();

        void Draw(sf::RenderWindow&);
    protected:
    private:
        sf::Sprite Shape;

        float Radius;
};

#endif // RUNWAY_H
