#include "Networker.hpp"

Networker::Networker() : Active(false)
{
    CleanConns();
}

Networker::~Networker()
{
    CleanConns();
}

bool Networker::SetupClient(const sf::IpAddress &NewIp)
{
    CleanConns();

    if (ClientSocket.Tcp.connect(NewIp, Port) != sf::Socket::Done)
        return false;

    if (Udp.bind(sf::Socket::AnyPort) != sf::Socket::Done)
    {
        ClientSocket.Tcp.disconnect();
        return false;
    }

    Server = false;
    Active = true;

    ClientSocket.Ip = NewIp;
    ClientSocket.UdpPort = Port;

    Selector.add(ClientSocket.Tcp);
    Selector.add(Udp);
    return true;
}

bool Networker::SetupServer()
{
    CleanConns();

    if (ServerTcp.listen(Port) != sf::Socket::Done)
        return false;

    if (Udp.bind(Port) != sf::Socket::Done)
    {
        ServerTcp.close();
        return false;
    }

    Server = true;
    Active = true;

    Selector.add(ServerTcp);
    Selector.add(Udp);
    return true;
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
            if (Pair.Id != SourceId && Pair.UdpPort != sf::Socket::AnyPort)
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
        if (Pair.Id == Id && Pair.UdpPort != sf::Socket::AnyPort)
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
                    Pair.UdpPort = sf::Socket::AnyPort;
                    ServerConns.push_back(NewPair);
                    Selector.add(Pair.Tcp);

                    sf::Packet NewPacket;
                    NewPacket << 0 << PacketTypes::ConnectionResponse << Pair.Id;
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
                    sf::Packet Info = Packet;
                    sf::Uint32 SourceId;
                    PacketType Type;
                    Info >> SourceId >> Type;

                    vector<ClientPair*>::iterator it;
                    for (it = ServerConns.begin(); it != ServerConns.end(); ++it)
                    {
                        ClientPair &Pair = **it;
                        if (Pair.Ip == Ip && Pair.Id == SourceId)
                        {
                            if (Type == PacketTypes::ConnectionUdp)
                            {
                                // Info >> Pair.UdpPort;
                                Pair.UdpPort = UdpPort;
                            }

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
                    else
                        ++it;
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
                        Info >> ClientSocket.Id;

                        SendUdp(sf::Packet() << ClientSocket.Id << PacketTypes::ConnectionUdp << Udp.getLocalPort());
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
