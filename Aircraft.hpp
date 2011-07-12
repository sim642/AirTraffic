#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "Math.hpp"
#include "Path.hpp"
#include "Runway.hpp"
#include "Explosion.hpp"

using namespace std;

struct AircraftTemplate
{
    string Name;
    string Res;
    float Radius;
    float Speed;
    vector<string> Runways;
};

class Aircraft
{
    public:
        Aircraft(AircraftTemplate, map<string, sf::Image>&, sf::Vector2f, float);
        AircraftTemplate GetTemplate();

        sf::Vector2f GetPos();

        Path& GetPath();
        bool OnMe(sf::Vector2f);

        bool Colliding(const Aircraft&);
        bool Colliding(const Explosion&);

        void SetRunway(Runway*);

        bool Step(float);
        void Draw(sf::RenderWindow&);
    protected:
    private:
        AircraftTemplate Template;

        float Radius;
        float Speed;
        float Turn;

        Path P;
        sf::Sprite Shape;

        Runway *Land;

        float Turning;
        bool FlyIn;

};

#endif // AIRCRAFT_H
