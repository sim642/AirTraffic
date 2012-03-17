#include "AirTrafficScreen.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Math.hpp"
#include "GraphUtil.hpp"

AirTrafficScreen::AirTrafficScreen(sf::RenderWindow &NewApp) : Screen(NewApp), Background(NULL)
{
    LoadResources();

    ScoreText.SetFont(Font);
    ScoreText.SetCharacterSize(40);
    ScoreText.SetPosition(sf::Vector2f(10.f, 10.f));
    ScoreText.SetColor(sf::Color::White);

    DebugText.SetFont(Font);
    DebugText.SetCharacterSize(24);
    DebugText.SetPosition(sf::Vector2f(10.f, 50.f));
    DebugText.SetColor(sf::Color::White);

    // not needed?
    //Reset();
}

AirTrafficScreen::~AirTrafficScreen()
{
    if (Background)
    {
        delete Background;
    }
}

AirTrafficScreen::ScreenType AirTrafficScreen::Run(const ScreenType &OldScreen)
{
    Running = true;
    while (Running)
    {
        HandleEvents();
        Step();
        Draw();
    }

    return MenuType;
}

void AirTrafficScreen::Reset()
{
    Pathing = NULL;
    Score = 0;

    Wind = sf::Vector2f(0.f, 0.f);

    Spawner = 0.f;
    PlayTime = 0.f;
    WindTime = 0.f;

    Aircrafts.clear();
    Runways.clear();
    Explosions.clear();
    Sceneries.clear();

    PickSurface();

    for (int n = 0; n < 2; n++)
    {
        SpawnRunway();
    }

    for (int n = 0; n < 2; n++)
    {
        SpawnScenery();
    }
}

void AirTrafficScreen::LoadResources()
{
    Sounds.insert(make_pair("", sf::SoundBuffer())); // empty sound for special cases

    Font.LoadFromFile("res/Play-Regular.ttf");

    boost::property_tree::json_parser::read_json("data.json", Data);

    boost::property_tree::ptree &DataRunways = Data.get_child("runways");
    boost::property_tree::ptree &DataAircrafts = Data.get_child("aircrafts");
    boost::property_tree::ptree &DataExplosions = Data.get_child("explosions");
    boost::property_tree::ptree &DataSceneries = Data.get_child("sceneries");
    boost::property_tree::ptree &DataSurfaces = Data.get_child("surfaces");

    for (boost::property_tree::ptree::iterator it = DataRunways.begin(); it != DataRunways.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        RunwayTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadTexture(Temp.Res);
        Temp.Center = sf::Vector2f(Cur.get<float>("centerx"), Cur.get<float>("centery"));
        Temp.Radius = Cur.get<float>("radius");
        Temp.LandAngle = Cur.get<float>("landangle");
        Temp.Length = Cur.get<float>("length");
        Temp.Directional = Cur.get<bool>("directional");

        RunwayTemplates.push_back(Temp);
    }

    for (boost::property_tree::ptree::iterator it = DataAircrafts.begin(); it != DataAircrafts.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        AircraftTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadTexture(Temp.Res);
        Temp.TakeoffRes = Cur.get("takeoffres", "");
        LoadSound(Temp.TakeoffRes);
        Temp.FlyRes = Cur.get("flyres", "");
        LoadSound(Temp.FlyRes);
        Temp.LandingRes = Cur.get("landingres", "");
        LoadSound(Temp.LandingRes);
        Temp.FrameSize = sf::Vector2i(Cur.get("framew", -1), Cur.get("frameh", -1));
        Temp.FrameRate = Cur.get("framerate", 0.f);
        Temp.Speed = Cur.get<float>("speed");
        Temp.Radius = Cur.get<float>("radius");
        Temp.Turn = Cur.get<float>("turn");

        boost::property_tree::ptree &CurRunways = Cur.get_child("runways");
        for (boost::property_tree::ptree::iterator it2 = CurRunways.begin(); it2 != CurRunways.end(); ++it2)
        {
            Temp.Runways.push_back(it2->second.get_value<string>());
        }

        AircraftTemplates.push_back(Temp);
    }

    for (boost::property_tree::ptree::iterator it = DataExplosions.begin(); it != DataExplosions.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        ExplosionTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadTexture(Temp.Res);
        Temp.Radius = Cur.get<float>("radius");
        Temp.Time = Cur.get<float>("time");

        ExplosionTemplates.push_back(Temp);
    }

    for (boost::property_tree::ptree::iterator it = DataSceneries.begin(); it != DataSceneries.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        SceneryTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadTexture(Temp.Res);
        Temp.FrameSize = sf::Vector2i(Cur.get("framew", -1), Cur.get("frameh", -1));
        Temp.FrameRate = Cur.get("framerate", 0.f);

        SceneryTemplates.push_back(Temp);
    }

    for (boost::property_tree::ptree::iterator it = DataSurfaces.begin(); it != DataSurfaces.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        SurfaceTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadTexture(Temp.Res);

        SurfaceTemplates.push_back(Temp);
    }
}

void AirTrafficScreen::HandleEvents()
{
    const sf::Vector2f MousePos(sf::Mouse::GetPosition(App).x, sf::Mouse::GetPosition(App).y);

    sf::Event Event;
    while (App.PollEvent(Event))
    {
        if (Event.Type == sf::Event::Closed || (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Keyboard::Escape))
            Running = false;

        if (Event.Type == sf::Event::MouseButtonPressed &&
            Event.MouseButton.Button == sf::Mouse::Left)
        {
            for (boost::ptr_list<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
            {
                if (it->Pathable() && it->OnMe(MousePos))
                {
                    Pathing = &*it;
                    Pathing->GetPath().Clear();
                    Pathing->GetPath().TryAddPoint(MousePos);

                    break;
                }
            }
        }
        else if (Event.Type == sf::Event::MouseButtonReleased &&
                 Event.MouseButton.Button == sf::Mouse::Left &&
                 Pathing)
        {
            Pathing = 0;
        }
        else if (Event.Type == sf::Event::MouseMoved && Pathing)
        {
            Path &P = Pathing->GetPath();
            if (P.TryAddPoint(MousePos))
            {
                Pathing->GetPath().Highlight = false;
                if (Pathing->GetDirection() == Aircraft::In)
                {
                    Runway *Land = 0;
                    for (boost::ptr_list<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
                    {
                        vector<string> Landable = Pathing->GetTemplate().Runways;
                        if (find(Landable.begin(), Landable.end(), it->GetTemplate().Name) != Landable.end() &&
                            it->OnMe(P[P.NumPoints() - 1]) &&
                            abs(AngleDiff(P.NumPoints() < 2 ? Pathing->GetAngle() : P.EndAngle(), it->GetAngle())) <= it->GetTemplate().LandAngle)
                        {
                            Land = &*it;
                            Pathing->GetPath().Highlight = true;
                            break;
                        }
                    }
                    Pathing->SetRunway(Land);
                }
                else
                {
                    sf::Vector2f Point = P[P.NumPoints() - 1];
                    Aircraft::OutDirections OutDirection = Pathing->GetOutDirection();

                    if ((OutDirection == Aircraft::OutUp && Point.y < 50) ||
                      (OutDirection == Aircraft::OutDown && Point.y > 550) ||
                      (OutDirection == Aircraft::OutLeft && Point.x < 50) ||
                      (OutDirection == Aircraft::OutRight && Point.x > 750))
                    {
                        Pathing->GetPath().Highlight = true;
                    }
                }
            }

        }
    }
}

void AirTrafficScreen::Step()
{
    const float FT = FrameTimer.Restart().AsSeconds();
    Spawner += FT;
    PlayTime += FT;
    WindTime += FT;

    sf::Vector2i MousePos = sf::Mouse::GetPosition(App);
    sf::Listener::SetPosition(MousePos.x, MousePos.y, 100.f);
    //sf::Listener::SetDirection(0.f, 0.f, -100.f);

    float SpawnTime = wr::Map(PlayTime, 0.f, 120.f, 5.f, 0.5f);
    if (SpawnTime < 0.5f)
        SpawnTime = 0.5f;

    if (Spawner > SpawnTime)
    {
        SpawnAircraft();
        Spawner = 0.f;
    }

    if (WindTime > 0.5f) // may need changing
    {
        if (Wind == sf::Vector2f(0.f, 0.f))
        {
            Wind = sf::Vector2f(5.f, 0.f);
            Wind = Rotate(Wind, Random(0.f, 360.f));
            Wind = Scale(Wind, Random(0.5f, 1.f));
        }
        else
        {
            Wind = Rotate(Wind, Random(-5.f, 5.f));
            Wind = Scale(Wind, Random(0.95f, 1.05f));
        }
        WindTime = 0.f;
    }

    for (boost::ptr_list<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); )
    {
        boost::ptr_list<Aircraft>::iterator it2 = it;
        for (++it2; it2 != Aircrafts.end(); ++it2)
        {
            if (it->Colliding(*it2))
            {
                break;
            }
        }

        boost::ptr_list<Explosion>::iterator it3 = Explosions.begin();
        for (; it3 != Explosions.end(); ++it3)
        {
            if (it->Colliding(*it3))
            {
                break;
            }
        }

        if (it2 != Aircrafts.end())
        {
            sf::Vector2f Pos1 = it->GetPos(), Pos2 = it2->GetPos();
            SpawnExplosion(Pos1);
            SpawnExplosion(Pos2);
            Score -= 10000;

            if (Pathing == &*it || Pathing == &*it2) // take care if were drawing a path
            {
                Pathing = 0;
            }

            Aircrafts.erase(it2);
            it = Aircrafts.erase(it);
        }
        else if (it3 != Explosions.end() && it3->Deadly())
        {
            sf::Vector2f Pos1 = it->GetPos(), Pos2 = it3->GetPos();
            SpawnExplosion(Pos1);
            Score -= 5000;

            if (Pathing == &*it)
            {
                Pathing = 0;
            }

            it = Aircrafts.erase(it);
        }
        else if (it->Step(FT, Wind))
        {
            Score += 1000;
            if (Pathing == &*it) // take care if were drawing a path
            {
                Pathing = 0;
            }

            it = Aircrafts.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (boost::ptr_list<Explosion>::iterator it = Explosions.begin(); it != Explosions.end();)
    {
        if (it->Step(FT))
        {
            it = Explosions.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (boost::ptr_list<Scenery>::iterator it = Sceneries.begin(); it != Sceneries.end(); ++it)
    {
        it->Step(FT);
    }

    ScoreText.SetString(boost::lexical_cast<string>(Score));
    DebugText.SetString(boost::lexical_cast<string>(SpawnTime));
}

void AirTrafficScreen::Draw()
{
    // surface
    Background->Draw(App);

    // sceneries
    for (boost::ptr_list<Scenery>::iterator it = Sceneries.begin(); it != Sceneries.end(); ++it)
    {
        it->Draw(App);
    }

    // runways
    for (boost::ptr_list<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
    {
        it->Draw(App);
    }
    // landing areas
    for (boost::ptr_list<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
    {
        if (Pathing)
        {
            if (Pathing->GetDirection() == Aircraft::In)
            {
                vector<string> Landable = Pathing->GetTemplate().Runways;
                if (find(Landable.begin(), Landable.end(), it->GetTemplate().Name) != Landable.end())
                {
                    App.Draw(Circle(it->GetPos(),
                                               it->GetTemplate().Radius - 3.f,
                                               sf::Color(0, 255, 255, 96),
                                               3.f,
                                               sf::Color(0, 255, 255)));
                }
            }
            else
            {
                switch (Pathing->GetOutDirection())
                {
                    case Aircraft::OutUp:
                        App.Draw(Rectangle(0.f, 0.f, 800.f, 50.f, sf::Color(0, 255, 255, 96), 3.f, sf::Color(0, 255, 255)));
                        break;
                    case Aircraft::OutDown:
                        App.Draw(Rectangle(0.f, 550.f, 800.f, 50.f, sf::Color(0, 255, 255, 96), 3.f, sf::Color(0, 255, 255)));
                        break;
                    case Aircraft::OutLeft:
                        App.Draw(Rectangle(0.f, 0.f, 50.f, 600.f, sf::Color(0, 255, 255, 96), 3.f, sf::Color(0, 255, 255)));
                        break;
                    case Aircraft::OutRight:
                        App.Draw(Rectangle(750.f, 0.f, 50.f, 600.f, sf::Color(0, 255, 255, 96), 3.f, sf::Color(0, 255, 255)));
                        break;
                }
            }
        }
    }

    // aircraft paths
    for (boost::ptr_list<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        it->GetPath().Draw(App);
    }
    // aircrafts
    for (boost::ptr_list<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        it->Draw(App);
    }
    // aircraft collision warnings
    for (boost::ptr_list<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        // with explosion
        for (boost::ptr_list<Explosion>::iterator it2 = Explosions.begin(); it2 != Explosions.end(); ++it2)
        {
            sf::Vector2f Pos1 = it->GetPos(), Pos2 = it2->GetPos();
            float R1 = it->GetRadius(), R2 = it2->GetRadius();
            float MaxDist = (R1 + R2) / 1.5f;
            float Dist = Distance(Pos1, Pos2);

            if (it2->Deadly() && Dist < MaxDist)
            {
                float MinDist = (R1 + R2) / 2.5f;
                App.Draw(Circle(Pos1,
                                           R1 - 3.f,
                                           sf::Color(255, 255, 0, wr::Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 255, 0, wr::Map<float>(Dist, MinDist, MaxDist, 255, 64))));
            }
        }
        // with other aircraft
        boost::ptr_list<Aircraft>::iterator it2 = it;
        for (++it2; it2 != Aircrafts.end(); ++it2)
        {
            sf::Vector2f Pos1 = it->GetPos(), Pos2 = it2->GetPos();
            float R1 = it->GetRadius(), R2 = it2->GetRadius();
            float MaxDist = 1.75 * (R1 + R2);
            float Dist = Distance(Pos1, Pos2);

            if (it->OnRunway() == it2->OnRunway() && Dist < MaxDist)
            {
                float MinDist = (R1 + R2) / 1.3f;
                App.Draw(Circle(Pos1,
                                           R1 - 3.f,
                                           sf::Color(255, 0, 0, wr::Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 0, 0, wr::Map<float>(Dist, MinDist, MaxDist, 255, 64))));
                App.Draw(Circle(Pos2,
                                           R2 - 3.f,
                                           sf::Color(255, 0, 0, wr::Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 0, 0, wr::Map<float>(Dist, MinDist, MaxDist, 255, 64))));
            }
        }
    }

    // explosions
    for (boost::ptr_list<Explosion>::iterator it = Explosions.begin(); it != Explosions.end(); ++it)
    {
        it->Draw(App);
    }

    // hud
    App.Draw(ScoreText);
    App.Draw(DebugText);

    App.Draw(Line(sf::Vector2f(750.f, 50.f), sf::Vector2f(750.f, 50.f) + Wind * 25.f, 3.f, sf::Color::White));
    App.Draw(Circle(sf::Vector2f(750.f, 50.f), 5.f, sf::Color::Red));

    App.Display();
}

void AirTrafficScreen::LoadTexture(const string &FileName)
{
    sf::Texture Texture;
    Texture.LoadFromFile("res/" + FileName);
    Textures.insert(make_pair(FileName, Texture));
}

void AirTrafficScreen::LoadSound(const string &FileName)
{
    if (!FileName.empty())
    {
        sf::SoundBuffer Sound;
        Sound.LoadFromFile("res/" + FileName);
        Sounds.insert(make_pair(FileName, Sound));
    }
}

void AirTrafficScreen::SpawnRunway()
{
    vector<RunwayTemplate>::iterator it = RunwayTemplates.begin();
    it += rand() % RunwayTemplates.size();
    RunwayTemplate &Temp = *it;

    sf::Vector2f Pos;
    float Angle;

    Runway *New;
    bool Ready;
    do
    {
        Ready = true;

        Pos.x = Random(200.f, 600.f);
        Pos.y = Random(200.f, 400.f);

        Angle = Random(0.f, 360.f);



        New = new Runway(Temp, Textures, Pos, Angle);
        for (boost::ptr_list<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
        {
            if (New->Colliding(*it))
            {
                Ready = false;
                break;
            }
        }
        if (!Ready)
        {
            delete New;
        }
    }
    while (!Ready);

    Runways.push_back(New);
}

void AirTrafficScreen::SpawnAircraft()
{
    vector<AircraftTemplate>::iterator it;
    bool CanLand = false;
    do
    {
        it = AircraftTemplates.begin();
        it += rand() % AircraftTemplates.size();

        for (boost::ptr_list<Runway>::iterator it2 = Runways.begin(); it2 != Runways.end(); ++it2)
        {
            vector<string> &Landable = it->Runways;
            if (find(Landable.begin(), Landable.end(), it2->GetTemplate().Name) != Landable.end())
            {
                CanLand = true;
                break;
            }
        }
    }
    while (!CanLand);

    AircraftTemplate &Temp = *it;

    sf::Vector2f Pos;
    float Angle;

    bool Ready;
    Aircraft* New;

    if (Chance(0.4f))
    {
        //takeoff
        vector<Runway*> Rws;

        for (boost::ptr_list<Runway>::iterator it2 = Runways.begin(); it2 != Runways.end(); ++it2)
        {
            vector<string> &Landable = Temp.Runways;
            if (find(Landable.begin(), Landable.end(), it2->GetTemplate().Name) != Landable.end())
            {
                Rws.push_back(&*it2);
            }
        }

        random_shuffle(Rws.begin(), Rws.end(), Shuffler);

        for (vector<Runway*>::iterator it2 = Rws.begin(); it2 != Rws.end(); ++it2)
        {
            Ready = true;
            Angle = (*it2)->GetAngle();
            Pos = (*it2)->GetPos();
            New = new Aircraft(Temp, Textures, Sounds, Pos, Angle, *it2);

            for (boost::ptr_list<Aircraft>::iterator it3 = Aircrafts.begin(); it3 != Aircrafts.end(); ++it3)
            {
                if (it3->Colliding(*New) || it3->GetLand() == *it2)
                {
                    Ready = false;
                    break;
                }
            }

            if (Ready)
            {
                break;
            }
            else
            {
                delete New;
            }
        }
    }
    else
    {
        do
        {
            Ready = true;

            Pos.x = Random(-25.f, 825.f);
            Pos.y = Random(-25.f, 625.f);
            Angle = 0.f;

            if (Chance(0.5f))
            {
                if (Chance(0.5f))
                {
                    Pos.y = -25.f;
                }
                else
                {
                    Pos.y = 625.f;
                }
            }
            else
            {
                if (Chance(0.5f))
                {
                    Pos.x = -25.f;
                }
                else
                {
                    Pos.x = 825.f;
                }
            }

            New = new Aircraft(Temp, Textures, Sounds, Pos, Angle);
            for (boost::ptr_list<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
            {
                if (it->Colliding(*New))
                {
                    Ready = false;
                    break;
                }
            }

            if (!Ready)
            {
                delete New;
            }
        }
        while (!Ready);
    }

    if (Ready)
    {
        Aircrafts.push_back(New);
    }
}

void AirTrafficScreen::SpawnExplosion(sf::Vector2f Pos)
{
    vector<ExplosionTemplate>::iterator it = ExplosionTemplates.begin();
    it += rand() % ExplosionTemplates.size();
    ExplosionTemplate &Temp = *it;
    Explosions.push_back(new Explosion(Temp, Textures, Pos));
}

void AirTrafficScreen::SpawnScenery()
{
    vector<SceneryTemplate>::iterator it = SceneryTemplates.begin();
    it += rand() % SceneryTemplates.size();
    SceneryTemplate &Temp = *it;

    sf::Vector2f Pos;
    float Angle;

    Scenery *New;
    bool Ready;
    do
    {
        Ready = true;

        Pos.x = Random(100.f, 700.f);
        Pos.y = Random(100.f, 500.f);

        Angle = Random(0.f, 360.f);

        New = new Scenery(Temp, Textures, Pos, Angle);
        for (boost::ptr_list<Scenery>::iterator it = Sceneries.begin(); it != Sceneries.end(); ++it)
        {
            if (New->Colliding(*it))
            {
                Ready = false;
                break;
            }
        }

        for (boost::ptr_list<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
        {
            if (New->Colliding(*it))
            {
                Ready = false;
                break;
            }
        }

        if (!Ready)
        {
            delete New;
        }
    }
    while (!Ready);

    Sceneries.push_back(New);
}

void AirTrafficScreen::PickSurface()
{
    if (Background)
    {
        delete Background;
    }

    vector<SurfaceTemplate>::iterator it = SurfaceTemplates.begin();
    it += rand() % SurfaceTemplates.size();
    SurfaceTemplate &Temp = *it;

    Background = new Surface(Temp, Textures);
}
