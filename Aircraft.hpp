#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
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

        Aircraft(AircraftTemplate NewTemplate, map<string, sf::Texture> &Textures, sf::Vector2f Pos, float Rot, Runway *NewRunway = 0);
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

        bool Colliding(const Aircraft &Other);
        bool Colliding(const Explosion &Exp);

        void SetRunway(Runway *NewLand);

        bool Step(float FT, sf::Vector2f Wind);
        void Draw(sf::RenderWindow &App);
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
