#ifndef SCREEN_H
#define SCREEN_H

#include <SFML/Graphics.hpp>

class Screen
{
    public:
        enum ScreenType
        {
            ExitType = -1,
            MenuType = 0,
            AirTrafficType
        };

        Screen(sf::RenderWindow &NewApp);

        virtual ScreenType Run(const ScreenType &OldScreen) = 0;
    protected:
        sf::RenderWindow &App;
};

#endif // SCREEN_H
