#ifndef AIRTRAFFIC_H
#define AIRTRAFFIC_H

#include <ctime>
#include <boost/ptr_container/ptr_vector.hpp>
#include <SFML/Graphics.hpp>
#include "Screen.hpp"
#include "AirTrafficScreen.hpp"
#include "MenuScreen.hpp"

class AirTraffic
{
    public:
        AirTraffic();

        int Run();

    private:
        sf::RenderWindow App;

        boost::ptr_vector<Screen> Screens;
        Screen::ScreenType CurScreen;
};

#endif // AIRTRAFFIC_H
