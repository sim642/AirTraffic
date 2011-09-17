#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <SFML/Graphics.hpp>
#include <vector>
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
    float Turn;
    vector<string> Runways;
};

class Aircraft
{
    public:
        Aircraft(AircraftTemplate, map<string, sf::Image>&, sf::Vector2f, float);
        AircraftTemplate GetTemplate();

        sf::Vector2f GetPos();
        float GetAngle();
        Runway* GetLand();
        float GetRadius();

        Path& GetPath();
        bool OnMe(sf::Vector2f);

        bool Pathable();
        bool OnRunway();

        bool Colliding(const Aircraft&);
        bool Colliding(const Explosion&);

        void SetRunway(Runway*);

        bool Step(float);
        void Draw(sf::RenderWindow&);
    protected:
    private:
        const AircraftTemplate Template;

        float Radius;
        float Speed;
        float Turn;

        Path P;
        sf::Sprite Shape;

        Runway *Land;

        float Turning;
        bool FlyIn;
        bool FlyRunway;
        sf::Vector2f LandPoint;
};

#endif // AIRCRAFT_H
