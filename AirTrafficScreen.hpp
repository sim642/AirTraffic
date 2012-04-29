#ifndef AIRTRAFFICSCREEN_H
#define AIRTRAFFICSCREEN_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <boost/ptr_container/ptr_list.hpp>
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
        void SetupClient(const string &Host);
        void SetupServer();
        void KillNet();

        void HandleNet();
    private:
        bool Running;
        sf::Clock FrameTimer;

        void LoadResources();
        void HandleEvents();
        void Step();
        void Draw();

        void Pause(bool Status);

        Networker Net;
        //void HandleNet();
        void SendGameData(const sf::Uint32 Id = 0);

        boost::property_tree::ptree Data;

        vector<AircraftTemplate> AircraftTemplates;
        vector<RunwayTemplate> RunwayTemplates;
        vector<ExplosionTemplate> ExplosionTemplates;
        vector<SceneryTemplate> SceneryTemplates;
        vector<SurfaceTemplate> SurfaceTemplates;

        boost::ptr_list<Aircraft> Aircrafts;
        boost::ptr_list<Runway> Runways;
        boost::ptr_list<Explosion> Explosions;
        boost::ptr_list<Scenery> Sceneries;

        Surface *Background;
        sf::Sound AlarmSound;

        Aircraft *Pathing;

        int Score;

        sf::Font Font;
        sf::Text ScoreText;
        sf::Text DebugText;

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
