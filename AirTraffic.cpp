#include "AirTraffic.hpp"
#include <ctime>
#include "AirTrafficScreen.hpp"
#include "MenuScreen.hpp"

AirTraffic::AirTraffic()
{
    srand(time(NULL));

    App.Create(sf::VideoMode(800, 600, 32), "AirTraffic", sf::Style::Default, sf::ContextSettings(0, 0, 16));
    App.SetFramerateLimit(60);

    Screens.push_back(new MenuScreen(App));
    Screens.push_back(new AirTrafficScreen(App));
}

int AirTraffic::Run()
{
    Screen::ScreenType NextScreen = Screen::MenuType;
    Screen::ScreenType OldScreen = Screen::ExitType;

    while (NextScreen != Screen::ExitType)
    {
        Screen::ScreenType CurScreen = NextScreen;
        NextScreen = Screens[NextScreen].Run(OldScreen);
        OldScreen = CurScreen;
    }

    App.Close();
    return 0;
}
