#include "AirTraffic.hpp"

AirTraffic::AirTraffic() : Pathing(), Score(0)
{
    srand(time(0));

    App.Create(sf::VideoMode(800, 600, 32), "AirTraffic", sf::Style::Default, sf::ContextSettings(24, 8, 16));
    App.SetFramerateLimit(60);

    LoadResources();

    ScoreText.SetFont(Font);
    ScoreText.SetCharacterSize(40);
    ScoreText.SetPosition(sf::Vector2f(10.f, 10.f));
    ScoreText.SetColor(sf::Color::White);

    DebugText.SetFont(Font);
    DebugText.SetCharacterSize(24);
    DebugText.SetPosition(sf::Vector2f(10.f, 50.f));
    DebugText.SetColor(sf::Color::White);

    for (int n = 0; n < 2; n++)
    {
        SpawnRunway();
    }
}

int AirTraffic::Run()
{
    while (App.IsOpened())
    {
        HandleEvents();
        Step();
        Draw();
    }

    return 0;
}

void AirTraffic::LoadResources()
{
    Font = sf::Font::GetDefaultFont();

    boost::property_tree::json_parser::read_json("data.json", Data);

    boost::property_tree::ptree &DataRunways = Data.get_child("runways");
    boost::property_tree::ptree &DataAircrafts = Data.get_child("aircrafts");
    boost::property_tree::ptree &DataExplosions = Data.get_child("explosions");

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

    GrassTexture.LoadFromFile("res/Grass192_2.png");
    GrassTexture.SetSmooth(false);
    Grass.SetTexture(GrassTexture);
}

void AirTraffic::HandleEvents()
{
    const sf::Vector2f MousePos(sf::Mouse::GetPosition(App).x, sf::Mouse::GetPosition(App).y);

    sf::Event Event;
    while (App.PollEvent(Event))
    {
        if (Event.Type == sf::Event::Closed)
            App.Close();

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
                    if (Point.x < 50 || Point.x > 750 || Point.y < 50 || Point.y > 550)
                    {
                        Pathing->GetPath().Highlight = true;
                    }
                }
            }

        }
    }
}

void AirTraffic::Step()
{
    const float FT = App.GetFrameTime() / 1000.f;
    float SpawnTime = wr::Map(PlayTime.GetElapsedTime() / 1000.f, 0.f, 120.f, 5.f, 0.5f);
    if (SpawnTime < 0.5f)
        SpawnTime = 0.5f;
    //float SpawnTime = 3.f / PlayTime.GetElapsedTime();

    if (Spawner.GetElapsedTime() / 1000.f > SpawnTime)
    {
        SpawnAircraft();
        Spawner.Reset();
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
        else if (it->Step(FT))
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

    ScoreText.SetString(boost::lexical_cast<string>(Score));
    DebugText.SetString(boost::lexical_cast<string>(SpawnTime));
}

void AirTraffic::Draw()
{
    // grass
    for (unsigned int y = 0; y < App.GetHeight(); y += GrassTexture.GetHeight())
    {
        for (unsigned int x = 0; x < App.GetWidth(); x += GrassTexture.GetWidth())
        {
            Grass.SetPosition(x, y);
            App.Draw(Grass);
        }
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
                App.Draw(Rectangle(0.f, 0.f, 800.f, 50.f, sf::Color(0, 255, 255, 96)));
                App.Draw(Rectangle(0.f, 550.f, 800.f, 50.f, sf::Color(0, 255, 255, 96)));
                App.Draw(Rectangle(0.f, 50.f, 50.f, 500.f, sf::Color(0, 255, 255, 96)));
                App.Draw(Rectangle(750.f, 50.f, 50.f, 500.f, sf::Color(0, 255, 255, 96)));

                App.Draw(Rectangle(50.f, 50.f, 700.f, 500.f, sf::Color(0, 0, 0, 0), 3.f, sf::Color(0, 255, 255)));
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

    App.Draw(ScoreText);
    App.Draw(DebugText);

    App.Display();
}

void AirTraffic::LoadTexture(const string &FileName)
{
    sf::Texture Texture;
    Texture.LoadFromFile("res/" + FileName);
    Textures.insert(make_pair(FileName, Texture));
}

void AirTraffic::SpawnRunway()
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
            sf::Vector2f Pos1 = New->GetPos(),
                         Pos2 = it->GetPos();
            if (InRange(Pos1, Pos2, 200.f))
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

void AirTraffic::SpawnAircraft()
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

        random_shuffle(Rws.begin(), Rws.end()); //needs moar random

        for (vector<Runway*>::iterator it2 = Rws.begin(); it2 != Rws.end(); ++it2)
        {
            Ready = true;
            Angle = (*it2)->GetAngle();
            Pos = (*it2)->GetPos();
            New = new Aircraft(Temp, Textures, Pos, Angle, *it2);

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

            New = new Aircraft(Temp, Textures, Pos, Angle);
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

void AirTraffic::SpawnExplosion(sf::Vector2f Pos)
{
    vector<ExplosionTemplate>::iterator it = ExplosionTemplates.begin();
    it += rand() % ExplosionTemplates.size();
    ExplosionTemplate &Temp = *it;
    Explosions.push_back(new Explosion(Temp, Textures, Pos));
}
