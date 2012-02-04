#include "MenuScreen.hpp"

MenuScreen::MenuScreen(sf::RenderWindow &NewApp, AirTrafficScreen *NewATC) : Screen(NewApp), ATC(NewATC), SelectionColor(200, 255, 150)
{
    GrassTex.LoadFromFile("res/Grass192_2.png");
    GrassSpr.SetTexture(GrassTex);

    sf::Image FadeImg;
    FadeImg.Create(App.GetWidth(), App.GetHeight(), sf::Color(0, 0, 0, 127));
    FadeTex.LoadFromImage(FadeImg);
    FadeSpr.SetTexture(FadeTex);

    Font.LoadFromFile("res/Play-Regular.ttf");
    FontBold.LoadFromFile("res/Play-Bold.ttf");

    TitleText.SetFont(FontBold);
    TitleText.SetCharacterSize(100);
    TitleText.SetString("AirTraffic");
    TitleText.SetPosition(50.f, 90.f);

    ItemText.SetFont(Font);
    ItemText.SetCharacterSize(50);

    Cursor.SetFont(Font);
    Cursor.SetCharacterSize(50);
    Cursor.SetString(">");
    Cursor.SetColor(SelectionColor);

    Items.push_back("New game");
    Items.push_back("Exit");
}

MenuScreen::ScreenType MenuScreen::Run(const ScreenType &OldScreen)
{
    ItemSelected = 0;
    bool Paused = OldScreen == AirTrafficType;

    if (Paused && Items.front() != "Continue")
    {
        Items.insert(Items.begin(), "Continue");
    }

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
            if (Event.Type == sf::Event::Closed)
            {
                return ExitType;
            }
            else if (Event.Type == sf::Event::KeyPressed)
            {
                switch (Event.Key.Code)
                {
                    case sf::Keyboard::Return:
                    {
                        string Selected = Items[ItemSelected];
                        if (Selected == "Continue")
                        {
                            return AirTrafficType;
                        }
                        else if (Selected == "New game")
                        {
                            ATC->Reset();
                            return AirTrafficType;
                        }
                        else if (Selected == "Exit")
                        {
                            return ExitType;
                        }
                        break;
                    }

                    case sf::Keyboard::Escape:
                        if (Paused)
                        {
                            return AirTrafficType;
                        }
                        break;

                    case sf::Keyboard::Up:
                        if (--ItemSelected < 0)
                        {
                            ItemSelected += Items.size();
                        }
                        break;

                    case sf::Keyboard::Down:
                        ++ItemSelected %= Items.size(); // magic
                        break;

                    default:
                        break;
                }
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
        App.Draw(FadeSpr);

        App.Draw(TitleText);
        for (int i = 0; i < Items.size(); i++)
        {
            ItemText.SetPosition(100.f, 250.f + i * 70.f);
            ItemText.SetString(Items[i]);
            ItemText.SetColor(i == ItemSelected ? SelectionColor : sf::Color::White);
            App.Draw(ItemText);
        }

        Cursor.SetPosition(65.f, 250.f + ItemSelected * 70.f);
        App.Draw(Cursor);

        App.Display();
    }
}
