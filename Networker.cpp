#include "Networker.hpp"

#include <iostream>
using namespace std;

Networker::Networker() : Active(false)
{
    CleanConns();
}

Networker::~Networker()
{
    CleanConns();
}

void Networker::SetupClient(const sf::IpAddress &NewIp)
{
    CleanConns();

    Server = false;
    Active = true;
    ClientSocket.Tcp.Connect(NewIp, Port);
    ClientSocket.Ip = NewIp;
    ClientSocket.UdpPort = Port;
    //Udp.Bind(Port); //temp

    Selector.Add(ClientSocket.Tcp);
    //Selector.Add(Udp);
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
}

void Networker::Kill()
{
    Active = false;
    CleanConns();
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
}

void Networker::SendTcp(sf::Packet &Packet, sf::Uint32 Id)
{
    if (Id == 0)
    {
        SendTcp(Packet);
        return;
    }

    for (vector<ClientPair*>::iterator it = ServerConns.begin(); it != ServerConns.end(); ++it)
    {
        ClientPair &Pair = **it;
        if (Pair.Id == Id)
        {
            Pair.Tcp.Send(Packet);
            break;
        }
    }
}

void Networker::SendUdp(sf::Packet &Packet)
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
                Udp.Send(Packet, Pair.Ip, Pair.UdpPort);
        }
    }
    else
        Udp.Send(Packet, ClientSocket.Ip, ClientSocket.UdpPort);
}

void Networker::SendUdp(sf::Packet &Packet, sf::Uint32 Id)
{
    if (Id == 0)
    {
        SendUdp(Packet);
        return;
    }

    for (vector<ClientPair*>::iterator it = ServerConns.begin(); it != ServerConns.end(); ++it)
    {
        ClientPair &Pair = **it;
        if (Pair.Id == Id)
        {
            Udp.Send(Packet, Pair.Ip, Pair.UdpPort);
            break;
        }
    }
}

Networker::ReceiveStatus Networker::Receive(sf::Packet &Packet)
{
    Packet.Clear();
    if (Active && Selector.Wait(sf::Milliseconds(1)))
    {
        if (Server)
        {
            if (Selector.IsReady(ServerTcp))
            {
                ClientPair *NewPair = new ClientPair;
                if (ServerTcp.Accept(NewPair->Tcp) == sf::Socket::Done)
                {
                    ClientPair &Pair = *NewPair;
                    Pair.Id = ServerConns.empty() ? 1 : ServerConns.back()->Id + 1;
                    Pair.Ip = Pair.Tcp.GetRemoteAddress();
                    Pair.UdpPort = Port + Pair.Id;
                    ServerConns.push_back(NewPair);
                    Selector.Add(Pair.Tcp);

                    sf::Packet NewPacket;
                    NewPacket << 0 << PacketTypes::ConnectionResponse << Pair.Id << Pair.UdpPort;
                    SendTcp(NewPacket, Pair.Id);

                    Packet << Pair.Id;
                    return Connected;
                }
                else
                    delete NewPair;
            }
            else if (Selector.IsReady(Udp))
            {
                sf::IpAddress Ip;
                sf::Uint16 UdpPort;
                sf::Socket::Status Status = Udp.Receive(Packet, Ip, UdpPort); // need to validate source ip
                if (Status == sf::Socket::Done)
                {
                    return NewPacket;
                }
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
                            return NewPacket;
                        }
                        else if (Status == sf::Socket::Disconnected)
                        {
                            Selector.Remove(Pair.Tcp);
                            it = ServerConns.erase(it);

                            Packet << Pair.Id;
                            return Disconnected;
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
                        sf::Uint16 UdpPort;
                        Info >> ClientSocket.Id >> UdpPort;
                        Udp.Bind(UdpPort);
                        Selector.Add(Udp);
                    }
                    return NewPacket;
                }
                else if (Status == sf::Socket::Disconnected)
                {
                    Kill();

                    Packet << 0;
                    return Disconnected;
                }
            }
            else if (Selector.IsReady(Udp))
            {
                sf::IpAddress Ip;
                sf::Uint16 UdpPort;
                sf::Socket::Status Status = Udp.Receive(Packet, Ip, UdpPort); // need to validate source ip
                if (Status == sf::Socket::Done)
                {
                    return NewPacket;
                }
            }
        }
    }
    return NoPacket;
}

bool Networker::IsActive()
{
    return Active;
}

bool Networker::IsServer()
{
    return Active && Server;
}

bool Networker::IsClient()
{
    return Active && !Server;
}

sf::Uint32 Networker::GetId()
{
    return Server ? 0 : ClientSocket.Id;
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
