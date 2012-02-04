#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "Screen.hpp"

class MenuScreen : public Screen
{
    public:
        MenuScreen(sf::RenderWindow &NewApp);

        virtual ScreenType Run(const ScreenType &OldScreen);

    private:
        sf::Texture GrassTex;
        sf::Sprite GrassSpr;
};

#endif // MENUSCREEN_H
