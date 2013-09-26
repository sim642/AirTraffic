#include "MenuScreen.hpp"
#include <cctype>

MenuScreen::MenuScreen(sf::RenderWindow &NewApp, AirTrafficScreen *NewATS) : Screen(NewApp), ATS(NewATS), Input(false), SelectionColor(200, 255, 150)
{
    GrassTex.loadFromFile("res/Grass192_2.png");
    GrassSpr.setTexture(GrassTex);

    sf::Image FadeImg;
    FadeImg.create(App.getSize().x, App.getSize().y, sf::Color(0, 0, 0, 127));
    FadeTex.loadFromImage(FadeImg);
    FadeSpr.setTexture(FadeTex);

    Font.loadFromFile("res/Play-Regular.ttf");
    FontBold.loadFromFile("res/Play-Bold.ttf");

    TitleText.setFont(FontBold);
    TitleText.setCharacterSize(100);
    TitleText.setString("AirTraffic");
    TitleText.setPosition(50.f, 90.f);

    QuestionText.setFont(FontBold);
    QuestionText.setCharacterSize(50);
    QuestionText.setPosition(100.f, 250.f);

    UserText.setFont(Font);
    UserText.setCharacterSize(50);
    UserText.setPosition(170.f, 300.f);

    CaretText.setFont(Font);
    CaretText.setCharacterSize(50);
    CaretText.setString("|");

    ItemText.setFont(Font);
    ItemText.setCharacterSize(50);

    Cursor.setFont(Font);
    Cursor.setCharacterSize(50);
    Cursor.setString(">");
    Cursor.setColor(SelectionColor);

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
        PauseTex.create(App.getSize().x, App.getSize().y);
        PauseTex.update(App);
    }
    sf::Sprite PauseSpr(PauseTex);


    while (1)
    {
        bool ItemPicked = false;

        sf::Event Event;
        while (App.pollEvent(Event))
        {
            if (Event.type == sf::Event::Closed)
            {
                return ExitType;
            }
            else if (Event.type == sf::Event::KeyPressed)
            {
                if (Input)
                {
                    switch (Event.key.code)
                    {
                        case sf::Keyboard::Return:
                        {
                            Input = false;

                            if (ATS->SetupClient(UserText.getString()))
                            {
                                AddItemBefore("Disconnect", "Join game");
                                RemoveItem("Join game");
                                RemoveItem("Host server");
                                RemoveItem("New game");

                                ATS->SetupClient(UserText.getString());
                                return AirTrafficType;
                            }

                            break;
                        }

                        case sf::Keyboard::Escape:
                            Input = false;
                            break;

                        case sf::Keyboard::BackSpace:
                        {
                            if (UserTypePos > 0)
                            {
                                sf::String Str = UserText.getString();
                                Str.erase(UserTypePos - 1);
                                UserText.setString(Str);
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
                            if (UserTypePos < UserText.getString().getSize())
                            {
                                UserTypePos++;
                            }

                        default:
                            break;
                    }
                }
                else
                {
                    switch (Event.key.code)
                    {
                        case sf::Keyboard::Return:
                        {
                            ItemPicked = true;
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
            else if (Input && Event.type == sf::Event::TextEntered && iswprint(Event.text.unicode))
            {
                sf::String Str = UserText.getString();
                Str.insert(UserTypePos, Event.text.unicode);
                UserText.setString(Str);
                UserTypePos++;
            }
            else if (Event.type == sf::Event::MouseMoved || Event.type == sf::Event::MouseButtonPressed)
            {
                for (unsigned int i = 0; i < Items.size(); i++)
                {
                    ItemText.setPosition(100.f, 220.f + i * 70.f);
                    ItemText.setString(Items[i]);

                    sf::Vector2f MousePos;
                    if (Event.type == sf::Event::MouseMoved)
                    {
                        MousePos = sf::Vector2f(Event.mouseMove.x, Event.mouseMove.y);
                    }
                    else if (Event.type == sf::Event::MouseButtonPressed)
                    {
                        MousePos = sf::Vector2f(Event.mouseButton.x, Event.mouseButton.y);
                    }

                    if (ItemText.getGlobalBounds().contains(MousePos))
                    {
                        ItemSelected = i;
                        if (Event.type == sf::Event::MouseButtonPressed)
                        {
                            ItemPicked = true;
                        }

                        break;
                    }
                }
            }
        }

        if (!ATS->IsNetActive() && HasItem("Disconnect"))
        {
            ItemPicked = true;
            ItemSelected = find(Items.begin(), Items.end(), "Disconnect") - Items.begin();
        }

        if (ItemPicked)
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
                QuestionText.setString("Host: ");
                UserText.setString("");
                UserTypePos = 0;
            }
            else if (Selected == "Host server")
            {
                if (ATS->SetupServer())
                {
                    AddItemBefore("Disconnect", "Join game");
                    RemoveItem("Join game");
                    RemoveItem("Host server");

                    return AirTrafficType;
                }
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
        }

        ATS->StepNet();

        App.clear();

        if (Paused)
        {
            ATS->Draw();
        }
        else
        {
            for (unsigned int y = 0; y < App.getSize().y; y += GrassTex.getSize().y)
            {
                for (unsigned int x = 0; x < App.getSize().x; x += GrassTex.getSize().x)
                {
                    GrassSpr.setPosition(x, y);
                    App.draw(GrassSpr);
                }
            }
        }
        App.draw(FadeSpr);

        App.draw(TitleText);

        if (Input)
        {
            App.draw(QuestionText);
            App.draw(UserText);
            CaretText.setPosition(UserText.findCharacterPos(UserTypePos) + sf::Vector2f(-8.f, 2.f));
            App.draw(CaretText);
        }
        else
        {
            for (unsigned int i = 0; i < Items.size(); i++)
            {
                ItemText.setPosition(100.f, 220.f + i * 70.f);
                ItemText.setString(Items[i]);
                ItemText.setColor(i == ItemSelected ? SelectionColor : sf::Color::White);
                App.draw(ItemText);
            }

            Cursor.setPosition(65.f, 220.f + ItemSelected * 70.f);
            App.draw(Cursor);
        }

        App.display();
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
