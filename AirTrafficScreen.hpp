#ifndef AIRTRAFFICSCREEN_H
#define AIRTRAFFICSCREEN_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/property_tree/ptree.hpp>
#include "Screen.hpp"
#include "Aircraft.hpp"
#include "Runway.hpp"
#include "Explosion.hpp"
#include "Scenery.hpp"
#include "Surface.hpp"
#include "Networker.hpp"

using namespace std;

class AirTrafficScreen : public Screen
{
    public:
        AirTrafficScreen(sf::RenderWindow &NewApp);
        ~AirTrafficScreen();

        virtual ScreenType Run(const ScreenType &OldScreen);

        void Reset();
        bool SetupClient(const string &Host);
        bool SetupServer();
        void KillNet();
        bool IsNetActive();

        void StepNet();
        void Draw();
    private:
        bool Running;
        sf::Clock FrameTimer;

        void LoadResources();
        void HandleEvents();
        void Step();

        void Pause(bool Status);

        Networker Net;
        void HandleNet();
        void SendGameData(const sf::Uint32 Id = 0);

        boost::property_tree::ptree Data;

        map<string, AircraftTemplate> AircraftTemplates;
        map<string, RunwayTemplate> RunwayTemplates;
        map<string, ExplosionTemplate> ExplosionTemplates;
        map<string, SceneryTemplate> SceneryTemplates;
        map<string, SurfaceTemplate> SurfaceTemplates;

        boost::ptr_map<sf::Uint32, Aircraft> Aircrafts;
        sf::Uint32 AidNext;
        boost::ptr_list<Runway> Runways;
        boost::ptr_list<Explosion> Explosions;
        boost::ptr_list<Scenery> Sceneries;

        sf::ConvexShape AirportArea;

        Surface *Background;
        sf::Sound AlarmSound;

        map<sf::Uint32, sf::Vector2i> Pointers;

        Aircraft *Pathing;
        sf::Uint32 PathingAid;
        void PathingFinish(Aircraft *Ac);

        int Score;

        sf::Font Font;
        sf::Text ScoreText;
        sf::Text DebugText;

        sf::Text ChatText;
        vector<sf::String> ChatLines;
        sf::String UserChatLine;
        void AddChatLine(const sf::String &Line);

        float Spawner;
        float PlayTime;

        sf::Vector2f Wind;
        float WindTime;

        void LoadTexture(const string& FileName);
        map<string, sf::Texture> Textures;
        void LoadSound(const string& FileName);
        map<string, sf::SoundBuffer> Sounds;

        void SpawnRunway();
        void SpawnAircraft();
        void SpawnExplosion(sf::Vector2f);
        void SpawnScenery();
        void PickSurface();
        void CalculateHull();

        class
        {
            public:
                int operator() (const int &n)
                {
                    return rand() % n;
                }
        } Shuffler;
};

#endif // AIRTRAFFICSCREEN_H
