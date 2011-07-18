#ifndef AIRTRAFFIC_H
#define AIRTRAFFIC_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <iterator>
#include <ctime>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Aircraft.hpp"
#include "Runway.hpp"
#include "Explosion.hpp"
#include "Math.hpp"

#include <iostream>

using namespace std;

class AirTraffic
{
    public:
        AirTraffic();

        int Run();
    protected:
    private:
        void LoadResources();
        void HandleEvents();
        void Step();
        void Draw();

        sf::RenderWindow App;

        boost::property_tree::ptree Data;

        map<string, AircraftTemplate> AircraftTemplates;
        map<string, RunwayTemplate> RunwayTemplates;
        map<string, ExplosionTemplate> ExplosionTemplates;

        boost::ptr_vector<Aircraft> Aircrafts;
        boost::ptr_vector<Runway> Runways;
        boost::ptr_vector<Explosion> Explosions;

        Aircraft *Pathing;

        int Score;

        sf::Font Font;
        sf::Text ScoreText;
        sf::Text DebugText;

        sf::Clock Spawner;
        sf::Clock PlayTime;

        void LoadImage(const string&);
        map<string, sf::Image> Images;

        sf::Image GrassImage;
        sf::Sprite Grass;

        void SpawnRunway();
        void SpawnAircraft();
        void SpawnExplosion(sf::Vector2f);
};

#endif // AIRTRAFFIC_H
