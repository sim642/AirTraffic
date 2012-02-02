#ifndef AIRTRAFFICSCREEN_H
#define AIRTRAFFICSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Screen.hpp"
#include "Aircraft.hpp"
#include "Runway.hpp"
#include "Explosion.hpp"
#include "Math.hpp"
#include "GraphUtil.hpp"

using namespace std;

class AirTrafficScreen : public Screen
{
    public:
        AirTrafficScreen(sf::RenderWindow &NewApp);

        virtual ScreenType Run();
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

        sf::Clock Spawner;
        sf::Clock PlayTime;

        sf::Vector2f Wind;
        sf::Clock WindTime;

        void LoadTexture(const string&);
        map<string, sf::Texture> Textures;

        sf::Texture GrassTexture;
        sf::Sprite Grass;

        void SpawnRunway();
        void SpawnAircraft();
        void SpawnExplosion(sf::Vector2f);
};

#endif // AIRTRAFFICSCREEN_H
