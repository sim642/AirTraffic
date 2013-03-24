#include "Networker.hpp"

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
    ClientSocket.Tcp.connect(NewIp, Port);
    ClientSocket.Ip = NewIp;
    ClientSocket.UdpPort = Port;
    //Udp.Bind(Port); //temp

    Selector.add(ClientSocket.Tcp);
    //Selector.Add(Udp);
}

void Networker::SetupServer()
{
    CleanConns();

    Server = true;
    Active = true;
    ServerTcp.listen(Port);
    Udp.bind(Port); //temp

    Selector.add(ServerTcp);
    Selector.add(Udp);
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
                Pair.Tcp.send(Packet);
        }
    }
    else
        ClientSocket.Tcp.send(Packet);
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
            Pair.Tcp.send(Packet);
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
                Udp.send(Packet, Pair.Ip, Pair.UdpPort);
        }
    }
    else
        Udp.send(Packet, ClientSocket.Ip, ClientSocket.UdpPort);
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
            Udp.send(Packet, Pair.Ip, Pair.UdpPort);
            break;
        }
    }
}

Networker::ReceiveStatus Networker::Receive(sf::Packet &Packet)
{
    Packet.clear();
    if (Active && Selector.wait(sf::milliseconds(1)))
    {
        if (Server)
        {
            if (Selector.isReady(ServerTcp))
            {
                ClientPair *NewPair = new ClientPair;
                if (ServerTcp.accept(NewPair->Tcp) == sf::Socket::Done)
                {
                    ClientPair &Pair = *NewPair;
                    Pair.Id = ServerConns.empty() ? 1 : ServerConns.back()->Id + 1;
                    Pair.Ip = Pair.Tcp.getRemoteAddress();
                    Pair.UdpPort = Port + Pair.Id;
                    ServerConns.push_back(NewPair);
                    Selector.add(Pair.Tcp);

                    sf::Packet NewPacket;
                    NewPacket << 0 << PacketTypes::ConnectionResponse << Pair.Id << Pair.UdpPort;
                    SendTcp(NewPacket, Pair.Id);

                    Packet << Pair.Id;
                    return Connected;
                }
                else
                    delete NewPair;
            }
            else if (Selector.isReady(Udp))
            {
                sf::IpAddress Ip;
                sf::Uint16 UdpPort;
                sf::Socket::Status Status = Udp.receive(Packet, Ip, UdpPort);
                if (Status == sf::Socket::Done)
                {
                    vector<ClientPair*>::iterator it;
                    for (it = ServerConns.begin(); it != ServerConns.end(); ++it)
                    {
                        ClientPair &Pair = **it;
                        if (Pair.Ip == Ip)
                        {
                            return NewPacket;
                        }
                    }
                }
            }
            else
            {
                for (vector<ClientPair*>::iterator it = ServerConns.begin(); it != ServerConns.end();)
                {
                    ClientPair &Pair = **it;
                    if (Selector.isReady(Pair.Tcp))
                    {
                        sf::Socket::Status Status = Pair.Tcp.receive(Packet);
                        if (Status == sf::Socket::Done)
                        {
                            return NewPacket;
                        }
                        else if (Status == sf::Socket::Disconnected)
                        {
                            Selector.remove(Pair.Tcp);
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
            if (Selector.isReady(ClientSocket.Tcp))
            {
                sf::Socket::Status Status = ClientSocket.Tcp.receive(Packet);
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
                        Udp.bind(UdpPort);
                        Selector.add(Udp);
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
            else if (Selector.isReady(Udp))
            {
                sf::IpAddress Ip;
                sf::Uint16 UdpPort;
                sf::Socket::Status Status = Udp.receive(Packet, Ip, UdpPort);
                if (Status == sf::Socket::Done && Ip == ClientSocket.Ip)
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
    Selector.clear();
    ClientSocket.Id = 0;
    ClientSocket.Tcp.disconnect();
    ServerTcp.close();
    Udp.unbind();

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
