#include "MenuScreen.hpp"

MenuScreen::MenuScreen(sf::RenderWindow &NewApp) : Screen(NewApp)
{

}

MenuScreen::ScreenType MenuScreen::Run(const ScreenType &OldScreen)
{
    sf::Texture PauseTex;
    PauseTex.Create(App.GetWidth(), App.GetHeight());
    PauseTex.Update(App);
    sf::Sprite PauseSpr(PauseTex);

    while (1)
    {
        sf::Event Event;
        while (App.PollEvent(Event))
        {
            if (Event.Type == sf::Event::Closed || (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Keyboard::Escape))
            {
                return ExitType;
            }
            else if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Keyboard::Return)
            {
                return AirTrafficType;
            }
        }

        App.Clear(sf::Color::Blue);
        if (OldScreen == AirTrafficType)
        {
            App.Draw(PauseSpr);
        }
        App.Display();
    }
}
