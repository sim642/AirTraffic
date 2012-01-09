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
        enum States
        {
            FlyingIn,
            FlyingOut,
            FlyingFree,
            FlyingPath,
            Landing,
            TakingOff
        };
        enum Directions
        {
            In,
            Out
        };
        enum OutDirections
        {
            OutUp,
            OutDown,
            OutLeft,
            OutRight
        };

        Aircraft(AircraftTemplate, map<string, sf::Texture>&, sf::Vector2f, float, Runway* = 0);
        AircraftTemplate GetTemplate();

        sf::Vector2f GetPos();
        float GetAngle();
        Runway* GetLand();
        float GetRadius();

        Path& GetPath();
        bool OnMe(sf::Vector2f);

        bool Pathable();
        bool OnRunway() const;
        Directions GetDirection();
        OutDirections GetOutDirection();

        bool Colliding(const Aircraft&);
        bool Colliding(const Explosion&);

        void SetRunway(Runway*);

        bool Step(float, sf::Vector2f);
        void Draw(sf::RenderWindow&);
    private:
        const AircraftTemplate Template;

        float Radius;
        float Speed;
        float Turn;

        Path P;
        sf::Sprite Shape;

        Runway *Land;

        float Turning;
        States State;
        Directions Direction;
        OutDirections OutDirection;
        sf::Vector2f LandPoint;
};

#endif // AIRCRAFT_H
