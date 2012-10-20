#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <map>
#include "AnimSprite.hpp"
#include "Path.hpp"
#include "Runway.hpp"
#include "Explosion.hpp"

using namespace std;

struct AircraftTemplate
{
    string Name;
    string Res;
    string TakeoffRes;
    string FlyRes;
    string LandingRes;
    sf::Vector2i FrameSize;
    float FrameRate;
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

        Aircraft(const AircraftTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot);
        Aircraft(const AircraftTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot, Runway *NewRunway, OutDirections NewOutDirection);
        Aircraft(const AircraftTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot, Runway *NewRunway, States NewState, OutDirections NewOutDirection);
        Aircraft(const AircraftTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot, Runway *NewRunway, States NewState, sf::Vector2f NewLandPoint);
        const AircraftTemplate& GetTemplate() const;

        sf::Vector2f GetPos() const;
        float GetAngle() const;
        Runway* const GetLand() const;
        float GetRadius() const;

        Path& GetPath();
        bool OnMe(sf::Vector2f);

        bool Pathable();
        bool OnRunway() const;
        States GetState();
        Directions GetDirection();
        OutDirections GetOutDirection();
        sf::Vector2f GetLandPoint();

        sf::Vector2f GetVelocity();

        bool Colliding(const Aircraft &Other) const;
        bool Colliding(const Explosion &Exp) const;

        void SetRunway(Runway *NewLand);

        void Pause(bool Status);

        bool Step(float FT, sf::Vector2f Wind);
        void Draw(sf::RenderWindow &App);
        void DrawShadow(sf::RenderWindow &App);

        void SetPitch(float Pitch);
    private:
        void Setup(map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot);

        const AircraftTemplate &Template;

        float Radius;
        float Speed;
        float Turn;

        Path P;
        AnimSprite Shape;

        Runway *Land;

        States State;
        Directions Direction;
        OutDirections OutDirection;
        sf::Vector2f LandPoint;

        sf::Sound TakeoffSound, FlySound, LandingSound;
};

#endif // AIRCRAFT_H
