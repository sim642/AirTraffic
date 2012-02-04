#ifndef AIRTRAFFIC_H
#define AIRTRAFFIC_H

#include <boost/ptr_container/ptr_vector.hpp>
#include <SFML/Graphics.hpp>
#include "Screen.hpp"

class AirTraffic
{
    public:
        AirTraffic();

        int Run();

    private:
        sf::RenderWindow App;

        boost::ptr_vector<Screen> Screens;
};

#endif // AIRTRAFFIC_H
