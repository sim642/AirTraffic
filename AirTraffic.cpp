#include "AirTraffic.hpp"

AirTraffic::AirTraffic()
{
    srand(time(NULL));

    App.Create(sf::VideoMode(800, 600, 32), "AirTraffic", sf::Style::Default, sf::ContextSettings(0, 0, 16));
    App.SetFramerateLimit(60);

    Screens.push_back(new MenuScreen(App));
    Screens.push_back(new AirTrafficScreen(App));

    CurScreen = Screen::MenuType;
}

int AirTraffic::Run()
{
    while (CurScreen != Screen::ExitType)
    {
        CurScreen = Screens[CurScreen].Run();
    }

    App.Close();
    return 0;
}
