#include "AirTraffic.hpp"
#include <ctime>
#include "AirTrafficScreen.hpp"
#include "MenuScreen.hpp"

AirTraffic::AirTraffic()
{
    srand(time(NULL));

    App.create(sf::VideoMode(800, 600, 32), "AirTraffic", sf::Style::Default, sf::ContextSettings(0, 0, 16));
    App.setFramerateLimit(60);

    AirTrafficScreen *ATS = new AirTrafficScreen(App);
    Screens.push_back(new MenuScreen(App, ATS));
    Screens.push_back(ATS);
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

    App.close();
    return 0;
}
