#ifndef AIRTRAFFIC_H
#define AIRTRAFFIC_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/lexical_cast.hpp>
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

        boost::ptr_vector<Aircraft> Aircrafts;
        boost::ptr_vector<Runway> Runways;
        boost::ptr_vector<Explosion> Explosions;

        Aircraft *Pathing;

        int Score;

        sf::Font Font;
        sf::String ScoreText;
        sf::String DebugText;

        sf::Clock Spawner;
        sf::Clock PlayTime;


        void LoadImage(vector<sf::Image>&, const string&);

        vector<sf::Image> AircraftImages;
        vector<sf::Image> RunwayImages;
        vector<sf::Image> ExplosionImages;
        sf::Image GrassImage;
        sf::Sprite Grass;

        void SpawnRunway();
        void SpawnAircraft();
};

#endif // AIRTRAFFIC_H
