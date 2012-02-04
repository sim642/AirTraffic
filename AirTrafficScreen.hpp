#ifndef AIRTRAFFICSCREEN_H
#define AIRTRAFFICSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/property_tree/ptree.hpp>
#include "Screen.hpp"
#include "Aircraft.hpp"
#include "Runway.hpp"
#include "Explosion.hpp"

using namespace std;

class AirTrafficScreen : public Screen
{
    public:
        AirTrafficScreen(sf::RenderWindow &NewApp);

        virtual ScreenType Run(const ScreenType &OldScreen);
    private:
        bool Running;

        void LoadResources();
        void HandleEvents();
        void Step();
        void Draw();

        boost::property_tree::ptree Data;

        vector<AircraftTemplate> AircraftTemplates;
        vector<RunwayTemplate> RunwayTemplates;
        vector<ExplosionTemplate> ExplosionTemplates;

        boost::ptr_list<Aircraft> Aircrafts;
        boost::ptr_list<Runway> Runways;
        boost::ptr_list<Explosion> Explosions;

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

        sf::Texture GrassTexture;
        sf::Sprite Grass;

        void SpawnRunway();
        void SpawnAircraft();
        void SpawnExplosion(sf::Vector2f);
};

#endif // AIRTRAFFICSCREEN_H
