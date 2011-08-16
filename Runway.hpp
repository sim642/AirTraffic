#ifndef RUNWAY_H
#define RUNWAY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "Math.hpp"

using namespace std;

struct RunwayTemplate
{
    string Name;
    string Res;
    sf::Vector2f Center;
    float Radius;
    float LandAngle;
    float Length;
};

class Runway
{
    public:
        Runway(RunwayTemplate, map<string, sf::Image>&, sf::Vector2f, float);
        RunwayTemplate GetTemplate();

        bool OnMe(sf::Vector2f);

        sf::Vector2f GetPos();
        float GetAngle();
        float GetLength();

        void Draw(sf::RenderWindow&);
    protected:
    private:
        const RunwayTemplate Template;
        sf::Sprite Shape;

        float Radius;
};

#endif // RUNWAY_H
