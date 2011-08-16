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

    for (int n = 0; n < 3; n++)
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
        LoadImage(Temp.Res);
        Temp.Center = sf::Vector2f(Cur.get<float>("centerx"), Cur.get<float>("centery"));
        Temp.Radius = Cur.get<float>("radius");
        Temp.LandAngle = Cur.get<float>("landangle");
        Temp.Length = Cur.get<float>("length");

        RunwayTemplates.insert(make_pair(Cur.get<string>("name"), Temp));
    }

    for (boost::property_tree::ptree::iterator it = DataAircrafts.begin(); it != DataAircrafts.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        AircraftTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadImage(Temp.Res);
        Temp.Speed = Cur.get<float>("speed");
        Temp.Radius = Cur.get<float>("radius");
        Temp.Turn = Cur.get<float>("turn");

        boost::property_tree::ptree &CurRunways = Cur.get_child("runways");
        for (boost::property_tree::ptree::iterator it2 = CurRunways.begin(); it2 != CurRunways.end(); ++it2)
        {
            Temp.Runways.push_back(it2->second.get_value<string>());
        }

        AircraftTemplates.insert(make_pair(Cur.get<string>("name"), Temp));
    }

    for (boost::property_tree::ptree::iterator it = DataExplosions.begin(); it != DataExplosions.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        ExplosionTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadImage(Temp.Res);
        Temp.Radius = Cur.get<float>("radius");
        Temp.Time = Cur.get<float>("time");

        ExplosionTemplates.insert(make_pair(Cur.get<string>("name"), Temp));
    }

    GrassImage.LoadFromFile("res/Grass192_2.png");
    GrassImage.SetSmooth(false);
    Grass.SetImage(GrassImage);
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
            for (boost::ptr_vector<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
            {
                if (it->Pathable() && it->OnMe(MousePos))
                {
                    Pathing = &*it;
                    Pathing->GetPath().Clear();

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
                Runway *Land = 0;
                for (boost::ptr_vector<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
                {
                    vector<string> Landable = Pathing->GetTemplate().Runways;
                    if (find(Landable.begin(), Landable.end(), it->GetTemplate().Name) != Landable.end() &&
                        it->OnMe(P[P.NumPoints() - 1]) &&
                        abs(AngleDiff(P.NumPoints() < 2 ? Pathing->GetAngle() : P.EndAngle(), it->GetAngle())) <= it->GetTemplate().LandAngle)
                    {
                        //cout << it->GetRotation() << " " << P.EndAngle() << " " << abs(fmod(AngleDiff(it->GetRotation(), P.EndAngle()), 360.f)) << endl;
                        Land = &*it;
                        break;
                    }
                }
                Pathing->SetRunway(Land);
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

    for (boost::ptr_vector<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); )
    {
        boost::ptr_vector<Aircraft>::iterator it2 = it + 1;
        for (; it2 != Aircrafts.end(); ++it2)
        {
            if (it->Colliding(*it2))
            {
                break;
            }
        }

        boost::ptr_vector<Explosion>::iterator it3 = Explosions.begin();
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
            //SpawnExplosion((Pos1 + Pos2) / 2.f);
            Score -= 10000;

            if (Pathing == &*it || Pathing == &*it2) // take care if were drawing a path
            {
                Pathing = 0;
            }

            if (it < it2)
            {
                //      --1--2--
                it = Aircrafts.erase(it);
                Aircrafts.erase(it2 - 1);
            }
            else
            {
                //      --2--1--
                Aircrafts.erase(it2);
                it = Aircrafts.erase(it - 1);
            }
        }
        else if (it3 != Explosions.end() && it3->Deadly())
        {
            sf::Vector2f Pos1 = it->GetPos(), Pos2 = it3->GetPos();
            SpawnExplosion(Pos1);
            //SpawnExplosion((Pos1 + Pos2) / 2.f);
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

    for (boost::ptr_vector<Explosion>::iterator it = Explosions.begin(); it != Explosions.end();)
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
    for (unsigned int y = 0; y < App.GetHeight(); y += GrassImage.GetHeight())
    {
        for (unsigned int x = 0; x < App.GetWidth(); x += GrassImage.GetWidth())
        {
            Grass.SetPosition(x, y);
            App.Draw(Grass);
        }
    }

    // runways
    for (boost::ptr_vector<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
    {
        it->Draw(App);
    }
    // landing areas
    for (boost::ptr_vector<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
    {
        if (Pathing)
        {
            vector<string> Landable = Pathing->GetTemplate().Runways;
            if (find(Landable.begin(), Landable.end(), it->GetTemplate().Name) != Landable.end())
            {
                App.Draw(sf::Shape::Circle(it->GetPos(),
                                           it->GetTemplate().Radius - 3.f,
                                           sf::Color(0, 255, 255, 96),
                                           3.f,
                                           sf::Color(0, 255, 255)));
            }
        }
    }

    // aircraft paths
    for (boost::ptr_vector<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        it->GetPath().Draw(App);
    }
    // aircrafts
    for (boost::ptr_vector<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        it->Draw(App);
    }
    // aircraft collision warnings
    for (boost::ptr_vector<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        // with explosion
        for (boost::ptr_vector<Explosion>::iterator it2 = Explosions.begin(); it2 != Explosions.end(); ++it2)
        {
            sf::Vector2f Pos1 = it->GetPos(), Pos2 = it2->GetPos();
            float R1 = it->GetRadius(), R2 = it2->GetRadius();
            float MaxDist = (R1 + R2) / 1.5f;
            float Dist = Distance(Pos1, Pos2);

            if (it2->Deadly() && Dist < MaxDist)
            {
                float MinDist = (R1 + R2) / 2.5f;
                App.Draw(sf::Shape::Circle(Pos1,
                                           R1 - 3.f,
                                           sf::Color(255, 255, 0, wr::Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 255, 0, wr::Map<float>(Dist, MinDist, MaxDist, 255, 64))));
            }
        }
        // with other aircraft
        for (boost::ptr_vector<Aircraft>::iterator it2 = it + 1; it2 != Aircrafts.end(); ++it2)
        {
            sf::Vector2f Pos1 = it->GetPos(), Pos2 = it2->GetPos();
            float R1 = it->GetRadius(), R2 = it2->GetRadius();
            float MaxDist = 1.75 * (R1 + R2);
            float Dist = Distance(Pos1, Pos2);

            if (it->OnRunway() == it2->OnRunway() && Dist < MaxDist)
            {
                float MinDist = (R1 + R2) / 1.3f;
                App.Draw(sf::Shape::Circle(Pos1,
                                           R1 - 3.f,
                                           sf::Color(255, 0, 0, wr::Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 0, 0, wr::Map<float>(Dist, MinDist, MaxDist, 255, 64))));
                App.Draw(sf::Shape::Circle(Pos2,
                                           R2 - 3.f,
                                           sf::Color(255, 0, 0, wr::Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 0, 0, wr::Map<float>(Dist, MinDist, MaxDist, 255, 64))));
            }
        }
    }

    // explosions
    for (boost::ptr_vector<Explosion>::iterator it = Explosions.begin(); it != Explosions.end(); ++it)
    {
        it->Draw(App);
    }

    App.Draw(ScoreText);
    App.Draw(DebugText);

    App.Display();
}

void AirTraffic::LoadImage(const string &FileName)
{
    sf::Image Image;
    Image.LoadFromFile("res/" + FileName);
    Images.insert(make_pair(FileName, Image));
}

void AirTraffic::SpawnRunway()
{
    map<string, RunwayTemplate>::iterator it = RunwayTemplates.begin();
    advance(it, rand() % RunwayTemplates.size());
    RunwayTemplate &Temp = it->second;

    sf::Vector2f Pos;
    float Angle;

    bool Ready;
    do
    {
        Ready = true;

        Pos.x = Random(200.f, 600.f);
        Pos.y = Random(200.f, 400.f);

        Angle = Random(0.f, 360.f);



        Runway New(Temp, Images, Pos, Angle);
        for (boost::ptr_vector<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
        {
            sf::Vector2f Pos1 = New.GetPos(),
                         Pos2 = it->GetPos();
            if (InRange(Pos1, Pos2, 200.f))
            {
                Ready = false;
                break;
            }
        }
    }
    while (!Ready);

    Runways.push_back(new Runway(Temp, Images, Pos, Angle));
}

void AirTraffic::SpawnAircraft()
{
    map<string, AircraftTemplate>::iterator it;
    bool CanLand = false;
    do
    {
        it = AircraftTemplates.begin();
        advance(it, rand() % AircraftTemplates.size());

        for (boost::ptr_vector<Runway>::iterator it2 = Runways.begin(); it2 != Runways.end(); ++it2)
        {
            vector<string> &Landable = it->second.Runways;
            if (find(Landable.begin(), Landable.end(), it2->GetTemplate().Name) != Landable.end())
            {
                CanLand = true;
                break;
            }
        }
    }
    while (!CanLand);

    AircraftTemplate &Temp = it->second;

    sf::Vector2f Pos;
    float Angle;

    bool Ready;
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

        Aircraft New(Temp, Images, Pos, Angle);
        for (boost::ptr_vector<Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
        {
            if (it->Colliding(New))
            {
                Ready = false;
                break;
            }
        }
    }
    while (!Ready);

    Aircrafts.push_back(new Aircraft(Temp, Images, Pos, Angle));
}

void AirTraffic::SpawnExplosion(sf::Vector2f Pos)
{
    map<string, ExplosionTemplate>::iterator it = ExplosionTemplates.begin();
    advance(it, rand() % ExplosionTemplates.size());
    ExplosionTemplate &Temp = it->second;
    Explosions.push_back(new Explosion(Temp, Images, Pos));
}
