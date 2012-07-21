#include "MenuScreen.hpp"
#include <cctype>

MenuScreen::MenuScreen(sf::RenderWindow &NewApp, AirTrafficScreen *NewATS) : Screen(NewApp), ATS(NewATS), Input(false), SelectionColor(200, 255, 150)
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

    QuestionText.SetFont(FontBold);
    QuestionText.SetCharacterSize(50);
    QuestionText.SetPosition(100.f, 250.f);

    UserText.SetFont(Font);
    UserText.SetCharacterSize(50);
    UserText.SetPosition(170.f, 300.f);

    CaretText.SetFont(Font);
    CaretText.SetCharacterSize(50);
    CaretText.SetString("|");

    ItemText.SetFont(Font);
    ItemText.SetCharacterSize(50);

    Cursor.SetFont(Font);
    Cursor.SetCharacterSize(50);
    Cursor.SetString(">");
    Cursor.SetColor(SelectionColor);

    AddItemEnd("New game");
    AddItemEnd("Join game");
    AddItemEnd("Host server");
    AddItemEnd("Exit");
}

MenuScreen::ScreenType MenuScreen::Run(const ScreenType &OldScreen)
{
    ItemSelected = 0;
    bool Paused = OldScreen == AirTrafficType;

    if (Paused && !HasItem("Continue"))
    {
        AddItemStart("Continue");
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
                if (Input)
                {
                    switch (Event.Key.Code)
                    {
                        case sf::Keyboard::Return:
                        {
                            Input = false;

                            AddItemBefore("Disconnect", "Join game");
                            RemoveItem("Join game");
                            RemoveItem("Host server");
                            RemoveItem("New game");

                            ATS->SetupClient(UserText.GetString());
                            return AirTrafficType;
                        }

                        case sf::Keyboard::Escape:
                            Input = false;
                            break;

                        case sf::Keyboard::Back:
                        {
                            if (UserTypePos > 0)
                            {
                                sf::String Str = UserText.GetString();
                                Str.Erase(UserTypePos - 1);
                                UserText.SetString(Str);
                                UserTypePos--;
                            }
                            break;
                        }

                        case sf::Keyboard::Left:
                            if (UserTypePos > 0)
                            {
                                UserTypePos--;
                            }
                            break;

                        case sf::Keyboard::Right:
                            if (UserTypePos < UserText.GetString().GetSize())
                            {
                                UserTypePos++;
                            }

                        default:
                            break;
                    }
                }
                else
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
                                ATS->Reset();
                                return AirTrafficType;
                            }
                            else if (Selected == "Join game")
                            {
                                Input = true;
                                QuestionText.SetString("Host: ");
                                UserText.SetString("");
                                UserTypePos = 0;
                            }
                            else if (Selected == "Host server")
                            {
                                AddItemBefore("Disconnect", "Join game");
                                RemoveItem("Join game");
                                RemoveItem("Host server");
                                ATS->SetupServer();
                                return AirTrafficType;
                            }
                            else if (Selected == "Disconnect")
                            {
                                if (!HasItem("New game"))
                                    AddItemAfter("New game", "Continue");
                                AddItemAfter("Join game", "Disconnect");
                                AddItemAfter("Host server", "Join game");
                                RemoveItem("Disconnect");
                                ItemSelected = 0;
                                ATS->KillNet();
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
                            ItemSelected = (ItemSelected == 0 ? Items.size() : ItemSelected) - 1;
                            break;

                        case sf::Keyboard::Down:
                            ++ItemSelected %= Items.size(); // magic
                            break;

                        default:
                            break;
                    }
                }
            }
            else if (Input && Event.Type == sf::Event::TextEntered && iswprint(Event.Text.Unicode))
            {
                sf::String Str = UserText.GetString();
                Str.Insert(UserTypePos, Event.Text.Unicode);
                UserText.SetString(Str);
                UserTypePos++;
            }
        }

        ATS->StepNet();

        App.Clear();

        if (Paused)
        {
            ATS->Draw();
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

        if (Input)
        {
            App.Draw(QuestionText);
            App.Draw(UserText);
            CaretText.SetPosition(UserText.FindCharacterPos(UserTypePos) + sf::Vector2f(-8.f, 2.f));
            App.Draw(CaretText);
        }
        else
        {
            for (unsigned int i = 0; i < Items.size(); i++)
            {
                ItemText.SetPosition(100.f, 250.f + i * 70.f);
                ItemText.SetString(Items[i]);
                ItemText.SetColor(i == ItemSelected ? SelectionColor : sf::Color::White);
                App.Draw(ItemText);
            }

            Cursor.SetPosition(65.f, 250.f + ItemSelected * 70.f);
            App.Draw(Cursor);
        }

        App.Display();
    }
}

void MenuScreen::AddItemStart(const string &Item)
{
    Items.insert(Items.begin(), Item);
}

void MenuScreen::AddItemEnd(const string &Item)
{
    Items.push_back(Item);
}

void MenuScreen::AddItemBefore(const string &Item, const string &Before)
{
    vector<string>::iterator it = find(Items.begin(), Items.end(), Before);
    if (it == Items.end())
        return;

    Items.insert(it, Item);
}

void MenuScreen::AddItemAfter(const string &Item, const string &After)
{
    vector<string>::iterator it = find(Items.begin(), Items.end(), After);
    if (it == Items.end())
        return;

    Items.insert(it + 1, Item);
}

void MenuScreen::RemoveItem(const string &Item)
{
    vector<string>::iterator it = find(Items.begin(), Items.end(), Item);
    if (it == Items.end())
        return;

    Items.erase(it);
}

bool MenuScreen::HasItem(const string &Item)
{
    vector<string>::iterator it = find(Items.begin(), Items.end(), Item);
    return it != Items.end();
}
