#include "Networker.hpp"

#include <iostream>
using namespace std;

Networker::Networker() : Active(false)
{
    CleanConns();
    cout << "Networker constructed" << endl;
}

Networker::~Networker()
{
    CleanConns();
    cout << "Networker destoried" << endl;
}

void Networker::SetupClient(const sf::IpAddress &NewIp)
{
    CleanConns();

    Server = false;
    Active = true;
    ClientSocket.Tcp.Connect(NewIp, Port);
    //Udp.Bind(Port); //temp

    Selector.Add(ClientSocket.Tcp);
    //Selector.Add(Udp);
    cout << "Client set up" << endl;
}

void Networker::SetupServer()
{
    CleanConns();

    Server = true;
    Active = true;
    ServerTcp.Listen(Port);
    Udp.Bind(Port); //temp

    Selector.Add(ServerTcp);
    Selector.Add(Udp);
    cout << "Server set up" << endl;
}

void Networker::Kill()
{
    Active = false;
    CleanConns();
    cout << "Networker killed" << endl;
}

void Networker::SendTcp(sf::Packet &Packet)
{
    if (Server)
    {
        sf::Packet Info = Packet;
        sf::Uint32 SourceId;
        Info >> SourceId;

        for (vector<ClientPair*>::iterator it = ServerConns.begin(); it != ServerConns.end(); ++it)
        {
            ClientPair &Pair = **it;
            if (Pair.Id != SourceId)
                Pair.Tcp.Send(Packet);
        }
    }
    else
        ClientSocket.Tcp.Send(Packet);

    cout << "Sent all" << endl;
}

void Networker::SendTcp(sf::Packet &Packet, sf::Uint32 Id)
{
    for (vector<ClientPair*>::iterator it = ServerConns.begin(); it != ServerConns.end(); ++it)
    {
        ClientPair &Pair = **it;
        if (Pair.Id == Id)
        {
            Pair.Tcp.Send(Packet);
            break;
        }
    }
    cout << "Sent one" << endl;
}

bool Networker::Receive(sf::Packet &Packet)
{
    if (Active && Selector.Wait(sf::Milliseconds(1)))
    {
        cout << "Got" << endl;
        if (Server)
        {
            if (Selector.IsReady(ServerTcp))
            {
                // new client
                ClientPair *NewPair = new ClientPair;
                if (ServerTcp.Accept(NewPair->Tcp) == sf::Socket::Done)
                {
                    ClientPair &Pair = *NewPair;
                    Pair.Id = ServerConns.empty() ? 1 : ServerConns.back()->Id + 1;
                    ServerConns.push_back(NewPair);
                    Selector.Add(Pair.Tcp);

                    sf::Packet NewPacket;
                    NewPacket << 0 << PacketTypes::ConnectionResponse << Pair.Id << Port + Pair.Id;
                    SendTcp(Packet, Pair.Id);
                }
                else
                    delete NewPair;
            }
            else if (Selector.IsReady(Udp))
            {

            }
            else
            {
                for (vector<ClientPair*>::iterator it = ServerConns.begin(); it != ServerConns.end();)
                {
                    ClientPair &Pair = **it;
                    if (Selector.IsReady(Pair.Tcp))
                    {
                        sf::Socket::Status Status = Pair.Tcp.Receive(Packet);
                        if (Status == sf::Socket::Done)
                        {
                            return true;
                        }
                        else if (Status == sf::Socket::Disconnected)
                        {
                            cout << "Disconnected " << Pair.Id << endl;
                            Selector.Remove(Pair.Tcp);
                            it = ServerConns.erase(it);
                            return false;
                        }
                        else
                            ++it;
                    }
                }
            }
        }
        else
        {
            if (Selector.IsReady(ClientSocket.Tcp))
            {
                sf::Socket::Status Status = ClientSocket.Tcp.Receive(Packet);
                if (Status == sf::Socket::Done)
                {
                    sf::Packet Info = Packet;
                    sf::Uint32 SourceId;
                    PacketType Type;
                    Info >> SourceId >> Type;
                    if (Type == PacketTypes::ConnectionResponse)
                    {
                        sf::Uint32 UdpPort;
                        Info >> ClientSocket.Id >> UdpPort;
                        Udp.Bind(UdpPort);
                        Selector.Add(Udp);
                    }
                    return true;
                }
                else if (Status == sf::Socket::Disconnected)
                {
                    cout << "Disconnected" << endl;
                    Kill();
                    return false;
                }
            }
            else if (Selector.IsReady(Udp))
            {

            }
        }
    }
    return false;
}

bool Networker::IsActive()
{
    return Active;
}

bool Networker::IsServer()
{
    return Server;
}

void Networker::CleanConns()
{
    Active = false;
    Server = false;
    Selector.Clear();
    ClientSocket.Id = 0;
    ClientSocket.Tcp.Disconnect();
    ServerTcp.Close();
    Udp.Unbind();

    for (vector<ClientPair*>::iterator it = ServerConns.begin(); it != ServerConns.end(); ++it)
        delete *it;
    ServerConns.clear();

    cout << "Connections cleared" << endl;
}

sf::Packet& operator>> (sf::Packet &Packet, PacketType &Type)
{
    sf::Uint32 Temp;
    Packet >> Temp;
    Type = static_cast<PacketType>(Temp);
    return Packet;
}

sf::Packet& operator<< (sf::Packet &Packet, PacketType &Type)
{
    Packet << static_cast<sf::Uint32>(Type);
    return Packet;
}
