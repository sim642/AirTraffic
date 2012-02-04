#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <vector>
#include "Screen.hpp"

using namespace std;

class MenuScreen : public Screen
{
    public:
        MenuScreen(sf::RenderWindow &NewApp);

        virtual ScreenType Run(const ScreenType &OldScreen);

    private:
        sf::Texture GrassTex;
        sf::Sprite GrassSpr;

        sf::Texture FadeTex;
        sf::Sprite FadeSpr;

        sf::Font Font;
        sf::Font FontBold;
        sf::Text TitleText;

        vector<string> Items;
        sf::Text ItemText;
        int ItemSelected;
        sf::Text Cursor;
        sf::Color SelectionColor;
};

#endif // MENUSCREEN_H
