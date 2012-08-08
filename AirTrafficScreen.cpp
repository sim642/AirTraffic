#include "AirTrafficScreen.hpp"
#include <iterator>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Math.hpp"
#include "GraphUtil.hpp"

#include <iostream>
using namespace std;

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

    AlarmSound.SetLoop(true);
    AlarmSound.SetRelativeToListener(true);

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
    Pause(false);

    FrameTimer.Restart();
    Running = true;
    while (Running)
    {
        HandleEvents();
        if (Net.IsActive())
            HandleNet();
        Step();
        App.Clear();
        Draw();
        App.Display();
    }

    Pause(true);

    return MenuType;
}

void AirTrafficScreen::StepNet()
{
    if (Net.IsActive())
    {
        HandleNet();
        Step();
    }
}

void AirTrafficScreen::Reset()
{
    Pathing = NULL;
    Score = 0;

    Wind = sf::Vector2f(0.f, 1.f);
    Wind = Rotate(Wind, Random(0.f, 360.f));
    Wind = Scale(Wind, Random(3.f, 10.f));

    Spawner = 0.f;
    PlayTime = 0.f;
    WindTime = 0.f;

    Aircrafts.clear();
    AidNext = 0;
    Runways.clear();
    Explosions.clear();
    Sceneries.clear();

    PickSurface();

    for (int n = 0; n < 3; n++)
    {
        SpawnRunway();
    }

    for (int n = 0; n < 5; n++)
    {
        SpawnScenery();
    }

    if (Net.IsServer())
    {
        SendGameData();
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

        RunwayTemplates.insert(make_pair(Temp.Name, Temp));
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

        AircraftTemplates.insert(make_pair(Temp.Name, Temp));
    }

    for (boost::property_tree::ptree::iterator it = DataExplosions.begin(); it != DataExplosions.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        ExplosionTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadTexture(Temp.Res);
        Temp.SoundRes = Cur.get("soundres", "");
        LoadSound(Temp.SoundRes);
        Temp.Radius = Cur.get<float>("radius");
        Temp.Time = Cur.get<float>("time");

        ExplosionTemplates.insert(make_pair(Temp.Name, Temp));
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

        SceneryTemplates.insert(make_pair(Temp.Name, Temp));
    }

    for (boost::property_tree::ptree::iterator it = DataSurfaces.begin(); it != DataSurfaces.end(); ++it)
    {
        boost::property_tree::ptree &Cur = it->second;
        SurfaceTemplate Temp;
        Temp.Name = Cur.get<string>("name");
        Temp.Res = Cur.get<string>("res");
        LoadTexture(Temp.Res);

        SurfaceTemplates.insert(make_pair(Temp.Name, Temp));
    }

    LoadTexture("Pointer.png");

    LoadSound("alarm.wav");
    AlarmSound.SetBuffer(Sounds["alarm.wav"]);
}

void AirTrafficScreen::SetupClient(const string &Host)
{
    Net.SetupClient(Host);
    Reset(); // not fully needed
}

void AirTrafficScreen::SetupServer()
{
    Net.SetupServer();
    Reset();
}

void AirTrafficScreen::KillNet()
{
    Net.Kill();
}

void AirTrafficScreen::HandleNet()
{
    sf::Packet Packet;
    Networker::ReceiveStatus ReceiveStatus;

    do
    {
        switch (ReceiveStatus = Net.Receive(Packet))
        {
            case Networker::Connected:
            {
                sf::Uint32 Id;
                Packet >> Id;
                cout << "Connected (" << Id << ")" << endl;
                if (Net.IsServer())
                {
                    SendGameData(Id);
                }
                break;
            }
            case Networker::Disconnected:
            {
                sf::Uint32 Id;
                Packet >> Id;
                cout << "Disconnected (" << Id << ")" << endl;

                Pointers.erase(Id);
                break;
            }
            case Networker::NewPacket:
            {
                sf::Uint32 SourceId;
                PacketType Type;
                Packet >> SourceId >> Type;
                cout << "Packet (Id " << SourceId << " Type " << Type << ")" << endl;
                switch (Type)
                {
                    case PacketTypes::SurfaceUpdate:
                    {
                        if (Net.IsServer())
                            break;
                        string Name;
                        Packet >> Name;

                        map<string, SurfaceTemplate>::iterator it = SurfaceTemplates.find(Name);

                        if (it != SurfaceTemplates.end())
                        {
                            if (Background)
                                delete Background;

                            Background = new Surface(it->second, Textures);
                        }
                        break;
                    }
                    case PacketTypes::SceneryUpdate:
                    {
                        if (Net.IsServer())
                            break;
                        Sceneries.clear();
                        while (!Packet.EndOfPacket())
                        {
                            string Name;
                            float X, Y, Angle;
                            Packet >> Name >> X >> Y >> Angle;

                            map<string, SceneryTemplate>::iterator it = SceneryTemplates.find(Name);

                            if (it != SceneryTemplates.end())
                                Sceneries.push_back(new Scenery(it->second, Textures, sf::Vector2f(X, Y), Angle));
                        }
                        break;
                    }
                    case PacketTypes::RunwayUpdate:
                    {
                        if (Net.IsServer())
                            break;
                        Runways.clear();
                        while (!Packet.EndOfPacket())
                        {
                            string Name;
                            float X, Y, Angle;
                            Packet >> Name >> X >> Y >> Angle;

                            map<string, RunwayTemplate>::iterator it = RunwayTemplates.find(Name);

                            if (it != RunwayTemplates.end())
                                Runways.push_back(new Runway(it->second, Textures, sf::Vector2f(X, Y), Angle));
                        }
                        break;
                    }
                    case PacketTypes::ScoreUpdate:
                    {
                        if (Net.IsServer())
                            break;
                        Packet >> Score;
                        break;
                    }
                    case PacketTypes::WindUpdate:
                    {
                        if (Net.IsServer())
                            break;
                        Packet >> Wind.x >> Wind.y;
                        break;
                    }
                    case PacketTypes::PointerUpdate:
                    {
                        sf::Vector2i PointerPos;
                        Packet >> PointerPos.x >> PointerPos.y;
                        Pointers[SourceId] = PointerPos;
                        break;
                    }
                    case PacketTypes::AircraftSpawn:
                    case PacketTypes::AircraftCreateOut:
                    case PacketTypes::AircraftCreateIn:
                    {
                        if (Net.IsServer())
                            break;

                        sf::Uint32 Aid;
                        string Name;
                        sf::Vector2f Pos;
                        float Angle;
                        sf::Int32 Rw;
                        Packet >> Aid >> Name >> Pos.x >> Pos.y >> Angle >> Rw;

                        map<string, AircraftTemplate>::iterator it = AircraftTemplates.find(Name);

                        if (it != AircraftTemplates.end())
                        {
                            Runway *Land = 0;
                            if (Rw >= 0)
                            {
                                boost::ptr_list<Runway>::iterator it2 = Runways.begin();
                                advance(it2, Rw);
                                Land = &*it2;
                            }

                            Aircraft *Ac;

                            switch (Type)
                            {
                                case PacketTypes::AircraftSpawn:
                                {
                                    if (Packet.EndOfPacket()) // in
                                    {
                                        Ac = new Aircraft(it->second, Textures, Sounds, Pos, Angle);
                                    }
                                    else // out
                                    {
                                        sf::Uint16 OutDirection;
                                        Packet >> OutDirection;
                                        Ac = new Aircraft(it->second, Textures, Sounds, Pos, Angle, Land, static_cast<Aircraft::OutDirections>(OutDirection));
                                    }

                                    break;
                                }
                                case PacketTypes::AircraftCreateOut:
                                {
                                    sf::Uint16 State;
                                    sf::Uint16 OutDirection;
                                    Packet >> State >> OutDirection;
                                    Ac = new Aircraft(it->second, Textures, Sounds, Pos, Angle, Land, static_cast<Aircraft::States>(State), static_cast<Aircraft::OutDirections>(OutDirection));
                                    break;
                                }
                                case PacketTypes::AircraftCreateIn:
                                {
                                    sf::Uint16 State;
                                    sf::Vector2f LandPoint;
                                    Packet >> State >> LandPoint.x >> LandPoint.y;
                                    Ac = new Aircraft(it->second, Textures, Sounds, Pos, Angle, Land, static_cast<Aircraft::States>(State), LandPoint);
                                    break;
                                }
                                default:
                                    break;
                            }

                            Aircrafts.insert(Aid, Ac);
                        }
                        break;
                    }
                    case PacketTypes::AircraftDestroy:
                    {
                        if (Net.IsServer())
                            break;

                        sf::Uint32 Aid;
                        Packet >> Aid;

                        boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.find(Aid);
                        if (it != Aircrafts.end())
                        {
                            Aircrafts.erase(it);
                        }
                        break;
                    }
                    case PacketTypes::ExplosionUpdate:
                    {
                        if (Net.IsServer())
                            break;

                        while (!Packet.EndOfPacket())
                        {
                            string Name;
                            sf::Vector2f Pos;
                            float Time;
                            Packet >> Name >> Pos.x >> Pos.y >> Time;

                            map<string, ExplosionTemplate>::iterator it = ExplosionTemplates.find(Name);

                            if (it != ExplosionTemplates.end())
                                Explosions.push_back(new Explosion(it->second, Textures, Sounds, Pos, Time));
                        }
                        break;
                    }
                    case PacketTypes::PathUpdate:
                    {
                        sf::Uint32 Aid;
                        sf::Vector2f Pos;
                        Packet >> Aid >> Pos.x >> Pos.y;

                        boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.find(Aid);
                        if (it != Aircrafts.end() && it->second->Pathable())
                        {
                            it->second->GetPath().TryAddPoint(Pos);
                            PathingFinish(it->second);
                        }

                        if (Net.IsServer())
                        {
                            Net.SendTcp(sf::Packet() << SourceId << PacketTypes::PathUpdate << Aid << Pos.x << Pos.y);
                        }
                        break;
                    }
                    case PacketTypes::PathClear:
                    {
                        sf::Uint32 Aid;
                        Packet >> Aid;

                        boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.find(Aid);
                        if (it != Aircrafts.end() && it->second->Pathable())
                        {
                            it->second->SetRunway(NULL);
                            it->second->GetPath().Clear();
                        }

                        if (Net.IsServer())
                        {
                            Net.SendTcp(sf::Packet() << SourceId << PacketTypes::PathClear << Aid);
                        }
                        break;
                    }
                }
                break;
            }
            case Networker::NoPacket:
            {
                break;
            }
        }
    }
    while (ReceiveStatus != Networker::NoPacket);
}

void AirTrafficScreen::SendGameData(const sf::Uint32 Id)
{
    Net.SendTcp(sf::Packet() << 0 << PacketTypes::ScoreUpdate << Score, Id); // score
    Net.SendTcp(sf::Packet() << 0 << PacketTypes::WindUpdate << Wind.x << Wind.y, Id); // wind
    Net.SendTcp(sf::Packet() << 0 << PacketTypes::SurfaceUpdate << Background->GetTemplate().Name, Id); // surface

    { // Runways
        sf::Packet Packet;
        Packet << 0 << PacketTypes::RunwayUpdate;
        for (boost::ptr_list<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
            Packet << it->GetTemplate().Name << it->GetPos().x << it->GetPos().y << it->GetAngle();
        Net.SendTcp(Packet, Id);
    }

    { // Scenery
        sf::Packet Packet;
        Packet << 0 << PacketTypes::SceneryUpdate;
        for (boost::ptr_list<Scenery>::iterator it = Sceneries.begin(); it != Sceneries.end(); ++it)
            Packet << it->GetTemplate().Name << it->GetPos().x << it->GetPos().y << it->GetAngle();
        Net.SendTcp(Packet, Id);
    }

    // Aircrafts
    for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        sf::Packet Packet;
        Packet << 0 << (it->second->GetDirection() == Aircraft::In ? PacketTypes::AircraftCreateIn : PacketTypes::AircraftCreateOut);
        Packet << it->first << it->second->GetTemplate().Name << it->second->GetPos().x << it->second->GetPos().y << it->second->GetAngle();
        if (it->second->GetLand() == NULL)
            Packet << -1;
        else
        {
            boost::ptr_list<Runway>::iterator it2 = Runways.begin();
            for (; it2 != Runways.end(); ++it2)
            {
                if (&*it2 == it->second->GetLand())
                    break;
            }
            sf::Int32 Rw = distance(Runways.begin(), it2);
            Packet << Rw;
        }
        Packet << static_cast<sf::Uint16>(it->second->GetState());

        if (it->second->GetDirection() == Aircraft::In)
        {
            const sf::Vector2f &LandPoint = it->second->GetLandPoint();
            Packet << LandPoint.x << LandPoint.y;
        }
        else
        {
            Packet << static_cast<sf::Uint16>(it->second->GetOutDirection());
        }

        Net.SendTcp(Packet, Id);
    }

    // Explosions
    {
        sf::Packet Packet;
        Packet << 0 << PacketTypes::ExplosionUpdate;

        for (boost::ptr_list<Explosion>::iterator it = Explosions.begin(); it != Explosions.end(); ++it)
        {
            const sf::Vector2f &Pos = it->GetPos();
            Packet << it->GetTemplate().Name << Pos.x << Pos.y << it->GetTime();
        }
        Net.SendTcp(Packet, Id);
    }
}

void AirTrafficScreen::HandleEvents()
{
    const sf::Vector2f MousePos(sf::Mouse::GetPosition(App).x, sf::Mouse::GetPosition(App).y);

    sf::Event Event;
    while (App.PollEvent(Event))
    {
        if (Event.Type == sf::Event::Closed ||
            (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Keyboard::Escape) /*||
            Event.Type == sf::Event::LostFocus*/)
        {
            Running = false;
        }

        if (Event.Type == sf::Event::MouseButtonPressed &&
            Event.MouseButton.Button == sf::Mouse::Left)
        {
            for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
            {
                Aircraft &Ac = *it->second;
                if (Ac.Pathable() && Ac.OnMe(MousePos))
                {
                    Pathing = &Ac;
                    PathingAid = it->first;
                    Pathing->SetRunway(NULL);
                    Pathing->GetPath().Clear();
                    Pathing->GetPath().TryAddPoint(MousePos);

                    if (Net.IsActive())
                    {
                        Net.SendTcp(sf::Packet() << Net.GetId() << PacketTypes::PathClear << PathingAid);
                        Net.SendTcp(sf::Packet() << Net.GetId() << PacketTypes::PathUpdate << PathingAid << MousePos.x << MousePos.y);
                    }

                    break;
                }
            }
        }
        else if (Event.Type == sf::Event::MouseButtonReleased &&
                 Event.MouseButton.Button == sf::Mouse::Left &&
                 Pathing != NULL)
        {
            Pathing = NULL;
        }
        else if (Event.Type == sf::Event::MouseMoved)
        {
            if (Net.IsActive())
            {
                Net.SendUdp(sf::Packet() << Net.GetId() << PacketTypes::PointerUpdate << Event.MouseMove.X << Event.MouseMove.Y);
            }

            if (Pathing != NULL)
            {
                Path &P = Pathing->GetPath();
                if (P.TryAddPoint(MousePos))
                {
                    if (Net.IsActive())
                    {
                        Net.SendTcp(sf::Packet() << Net.GetId() << PacketTypes::PathUpdate << PathingAid << MousePos.x << MousePos.y);
                    }
                    PathingFinish(Pathing);
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

    float SpawnTime = Map(PlayTime, 0.f, 120.f, 5.f, 0.5f);
    if (SpawnTime < 0.5f)
        SpawnTime = 0.5f;

    if ((!Net.IsActive() || Net.IsServer()) && Spawner > SpawnTime)
    {
        SpawnAircraft();
        Spawner = 0.f;
    }

    if ((!Net.IsActive() || Net.IsServer()) && WindTime > 0.5f) // may need changing
    {
        Wind = Rotate(Wind, Random(-5.f, 5.f));
        Wind *= Random(0.95f, 1.05f);
        WindTime = 0.f;

        Net.SendTcp(sf::Packet() << 0 << PacketTypes::WindUpdate << Wind.x << Wind.y);
    }

    for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); )
    {
        Aircraft &Ac = *it->second;

        if (!Net.IsActive() || Net.IsServer())
        {
            boost::ptr_map<sf::Uint32, Aircraft>::iterator it2 = it;
            for (++it2; it2 != Aircrafts.end(); ++it2)
            {
                if (Ac.Colliding(*it2->second))
                {
                    break;
                }
            }

            boost::ptr_list<Explosion>::iterator it3 = Explosions.begin();
            for (; it3 != Explosions.end(); ++it3)
            {
                if (Ac.Colliding(*it3))
                {
                    break;
                }
            }

            bool ScoreUpdate = false;

            if (it2 != Aircrafts.end())
            {
                sf::Vector2f Pos1 = Ac.GetPos(), Pos2 = it2->second->GetPos();
                SpawnExplosion(Pos1);
                SpawnExplosion(Pos2);
                Score -= 10000;
                ScoreUpdate = true;

                if (Pathing == it->second || Pathing == it2->second) // take care if were drawing a path
                {
                    Pathing = NULL;
                }

                Net.SendTcp(sf::Packet() << 0 << PacketTypes::AircraftDestroy << it->first);
                Net.SendTcp(sf::Packet() << 0 << PacketTypes::AircraftDestroy << it2->first);

                Aircrafts.erase(it2);
                it = Aircrafts.erase(it);
            }
            else if (it3 != Explosions.end() && it3->Deadly())
            {
                sf::Vector2f Pos1 = Ac.GetPos()/*, Pos2 = it3->GetPos()*/;
                SpawnExplosion(Pos1);
                Score -= 5000;
                ScoreUpdate = true;

                if (Pathing == it->second)
                {
                    Pathing = NULL;
                }

                Net.SendTcp(sf::Packet() << 0 << PacketTypes::AircraftDestroy << it->first);

                it = Aircrafts.erase(it);
            }
            else if (Ac.Step(FT, Wind))
            {
                Score += 1000;
                ScoreUpdate = true;

                if (Pathing == it->second) // take care if were drawing a path
                {
                    Pathing = NULL;
                }

                Net.SendTcp(sf::Packet() << 0 << PacketTypes::AircraftDestroy << it->first);

                it = Aircrafts.erase(it);
            }
            else
            {
                ++it;
            }

            if (ScoreUpdate)
            {
                Net.SendTcp(sf::Packet() << 0 << PacketTypes::ScoreUpdate << Score);
            }
        }
        else
        {
            Ac.Step(FT, Wind);
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
        if (Pathing != NULL)
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

    // aircraft shadows
    for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        it->second->DrawShadow(App);
    }

    // aircraft paths
    for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        it->second->GetPath().Draw(App);
    }
    // aircrafts
    for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        it->second->Draw(App);
    }
    // aircraft collision warnings
    bool AlarmOn = false;
    for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        const Aircraft &Ac = *it->second;

        // with explosion
        for (boost::ptr_list<Explosion>::iterator it2 = Explosions.begin(); it2 != Explosions.end(); ++it2)
        {
            sf::Vector2f Pos1 = Ac.GetPos(), Pos2 = it2->GetPos();
            float R1 = Ac.GetRadius(), R2 = it2->GetRadius();
            float MaxDist = (R1 + R2) / 1.5f;
            float Dist = Distance(Pos1, Pos2);

            if (it2->Deadly() && Dist < MaxDist)
            {
                AlarmOn = true;
                float MinDist = (R1 + R2) / 2.5f;
                App.Draw(Circle(Pos1,
                                           R1 - 3.f,
                                           sf::Color(255, 255, 0, Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 255, 0, Map<float>(Dist, MinDist, MaxDist, 255, 64))));
            }
        }
        // with other aircraft
        boost::ptr_map<sf::Uint32, Aircraft>::iterator it2 = it;
        for (++it2; it2 != Aircrafts.end(); ++it2)
        {
            const Aircraft &Ac2 = *it2->second;

            sf::Vector2f Pos1 = Ac.GetPos(), Pos2 = Ac2.GetPos();
            float R1 = Ac.GetRadius(), R2 = Ac2.GetRadius();
            float MaxDist = 1.75 * (R1 + R2);
            float Dist = Distance(Pos1, Pos2);

            if (Ac.OnRunway() == Ac2.OnRunway() && Dist < MaxDist)
            {
                AlarmOn = true;
                float MinDist = (R1 + R2) / 1.3f;
                App.Draw(Circle(Pos1,
                                           R1 - 3.f,
                                           sf::Color(255, 0, 0, Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 0, 0, Map<float>(Dist, MinDist, MaxDist, 255, 64))));
                App.Draw(Circle(Pos2,
                                           R2 - 3.f,
                                           sf::Color(255, 0, 0, Map<float>(Dist, MinDist, MaxDist, 128, 32)),
                                           3.f,
                                           sf::Color(255, 0, 0, Map<float>(Dist, MinDist, MaxDist, 255, 64))));
            }
        }
    }
    if (AlarmSound.GetStatus() == sf::Sound::Stopped && AlarmOn)
    {
        AlarmSound.Play();
    }
    else if (AlarmSound.GetStatus() == sf::Sound::Playing && !AlarmOn)
    {
        AlarmSound.Stop();
    }

    // explosions
    for (boost::ptr_list<Explosion>::iterator it = Explosions.begin(); it != Explosions.end(); ++it)
    {
        it->Draw(App);
    }

    // hud
    App.Draw(ScoreText);
    App.Draw(DebugText);

    App.Draw(Line(sf::Vector2f(750.f, 50.f), sf::Vector2f(750.f, 50.f) + Wind * 4.f, 3.f, sf::Color::White));
    App.Draw(Circle(sf::Vector2f(750.f, 50.f), 5.f, sf::Color::Red));

    // pointers
    for (map<sf::Uint32, sf::Vector2i>::iterator it = Pointers.begin(); it != Pointers.end(); ++it)
    {
        sf::Sprite Pointer(Textures["Pointer.png"]);
        Pointer.SetPosition(sf::Vector2f(it->second.x, it->second.y));
        App.Draw(Pointer);
    }
}

void AirTrafficScreen::Pause(bool Status)
{
    for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
    {
        it->second->Pause(Status);
    }

    for (boost::ptr_list<Explosion>::iterator it = Explosions.begin(); it != Explosions.end(); ++it)
    {
        it->Pause(Status);
    }

    if (Status)
    {
        AlarmSound.Pause();
    }
    else
    {
        if (AlarmSound.GetStatus() == sf::Sound::Paused)
        {
            AlarmSound.Play();
        }
    }
}

void AirTrafficScreen::PathingFinish(Aircraft *Ac)
{
    Path &P = Ac->GetPath();

    Ac->GetPath().Highlight = false;
    if (Ac->GetDirection() == Aircraft::In)
    {
        Runway *Land = 0;
        for (boost::ptr_list<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it)
        {
            vector<string> Landable = Ac->GetTemplate().Runways;
            if (find(Landable.begin(), Landable.end(), it->GetTemplate().Name) != Landable.end() &&
                it->OnMe(P[P.NumPoints() - 1]) &&
                abs(AngleDiff(P.NumPoints() < 2 ? Ac->GetAngle() : P.EndAngle(), it->GetAngle())) <= it->GetTemplate().LandAngle)
            {
                Land = &*it;
                Ac->GetPath().Highlight = true;
                break;
            }
        }
        Ac->SetRunway(Land);
    }
    else
    {
        sf::Vector2f Point = P[P.NumPoints() - 1];
        Aircraft::OutDirections OutDirection = Ac->GetOutDirection();

        if ((OutDirection == Aircraft::OutUp && Point.y < 50) ||
          (OutDirection == Aircraft::OutDown && Point.y > 550) ||
          (OutDirection == Aircraft::OutLeft && Point.x < 50) ||
          (OutDirection == Aircraft::OutRight && Point.x > 750))
        {
            Ac->GetPath().Highlight = true;
        }
    }
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
    map<string, RunwayTemplate>::iterator it = RunwayTemplates.begin();
    advance(it, rand() % RunwayTemplates.size());
    const RunwayTemplate &Temp = it->second;

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
    map<string, AircraftTemplate>::iterator it;
    bool CanLand = false;
    do
    {
        it = AircraftTemplates.begin();
        advance(it, rand() % AircraftTemplates.size());

        for (boost::ptr_list<Runway>::iterator it2 = Runways.begin(); it2 != Runways.end(); ++it2)
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

    const AircraftTemplate &Temp = it->second;

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
            const vector<string> &Landable = Temp.Runways;
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
            Aircraft::OutDirections OutDirection = static_cast<Aircraft::OutDirections>(Random(Aircraft::OutUp, Aircraft::OutRight)); //right order, shitty random
            New = new Aircraft(Temp, Textures, Sounds, Pos, Angle, *it2, OutDirection);

            for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it3 = Aircrafts.begin(); it3 != Aircrafts.end(); ++it3)
            {
                const Aircraft &Ac = *it3->second;

                if (Ac.Colliding(*New) || Ac.GetLand() == *it2)
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
            for (boost::ptr_map<sf::Uint32, Aircraft>::iterator it = Aircrafts.begin(); it != Aircrafts.end(); ++it)
            {
                if (it->second->Colliding(*New))
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
        sf::Uint32 Aid = AidNext++;
        Aircrafts.insert(Aid, New);

        sf::Int32 Rw = -1;
        if (New->GetLand() != 0)
        {
            Rw = 0;
            for (boost::ptr_list<Runway>::iterator it = Runways.begin(); it != Runways.end(); ++it, Rw++)
            {
                if (&*it == New->GetLand())
                    break;
            }
        }

        sf::Packet Packet;
        Packet << 0 << PacketTypes::AircraftSpawn << Aid << Temp.Name << New->GetPos().x << New->GetPos().y << New->GetAngle() << Rw;
        if (New->GetDirection() == Aircraft::Out)
        {
            Packet << static_cast<sf::Uint16>(New->GetOutDirection());
        }
        Net.SendTcp(Packet);
    }
}

void AirTrafficScreen::SpawnExplosion(sf::Vector2f Pos)
{
    map<string, ExplosionTemplate>::iterator it = ExplosionTemplates.begin();
    advance(it, rand() % ExplosionTemplates.size());
    const ExplosionTemplate &Temp = it->second;
    Explosions.push_back(new Explosion(Temp, Textures, Sounds, Pos));

    if (Net.IsServer())
    {
        Net.SendTcp(sf::Packet() << 0 << PacketTypes::ExplosionUpdate << Temp.Name << Pos.x << Pos.y << 0.f);
    }
}

void AirTrafficScreen::SpawnScenery()
{
    map<string, SceneryTemplate>::iterator it = SceneryTemplates.begin();
    advance(it, rand() % SceneryTemplates.size());
    const SceneryTemplate &Temp = it->second;

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

    map<string, SurfaceTemplate>::iterator it = SurfaceTemplates.begin();
    advance(it, rand() % SurfaceTemplates.size());
    const SurfaceTemplate &Temp = it->second;

    Background = new Surface(Temp, Textures);
}
