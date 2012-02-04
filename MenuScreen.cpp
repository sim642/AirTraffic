#include "MenuScreen.hpp"

MenuScreen::MenuScreen(sf::RenderWindow &NewApp) : Screen(NewApp)
{
    GrassTex.LoadFromFile("res/Grass192_2.png");
    GrassSpr.SetTexture(GrassTex);
}

MenuScreen::ScreenType MenuScreen::Run(const ScreenType &OldScreen)
{
    bool Paused = OldScreen == AirTrafficType;

    sf::Texture PauseTex;
    if (Paused)
    {
        PauseTex.Create(App.GetWidth(), App.GetHeight());
        PauseTex.Update(App);
    }
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

        App.Clear();

        if (Paused)
        {
            App.Draw(PauseSpr);
        }
        else
        {
            for (unsigned int y = 0; y < App.GetHeight(); y += GrassTex.GetHeight())
            {
                for (unsigned int x = 0; x < App.GetWidth(); x += GrassTex.GetWidth())
                {
                    GrassSpr.SetPosition(x, y);
                    App.Draw(GrassSpr);
                }
            }
        }

        App.Display();
    }
}
