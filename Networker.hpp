#ifndef NETWORKER_H
#define NETWORKER_H

#include <SFML/Network.hpp>
#include <vector>

using namespace std;

namespace PacketTypes
{
    enum PacketType
    {
        ConnectionResponse,
        SurfaceUpdate,
        SceneryUpdate,
        RunwayUpdate,
        ScoreUpdate,
        WindUpdate
    };
}
typedef PacketTypes::PacketType PacketType;

sf::Packet& operator>> (sf::Packet &Packet, PacketType &Type);
sf::Packet& operator<< (sf::Packet &Packet, PacketType &Type);

/** General packets
    sf::Uint32 SourceId, PacketType Type
**/
/** Packets
--0. ConnectionResponse--
    sf::Uint32 Id, sf::Uint32 UdpPort
--1. SurfaceUpdate--
    string Name
--2. SceneryUpdate--
    [string Name, float X, float Y, float Angle]
--3. RunwayUpdate--
    [string Name, float X, float Y, float Angle]
--4. ScoreUpdate--
    sf::Int32 Score
--5. WindUpdate--
    float X, float Y
**/

class Networker
{
    public:
        static const unsigned int Port = 4444;

        enum ReceiveStatus
        {
            Connected,
            Disconnected,
            NewPacket,
            NoPacket
        };

        Networker();
        ~Networker();

        void SetupClient(const sf::IpAddress &NewIp);
        void SetupServer();
        void Kill();

        void SendTcp(sf::Packet &Packet);
        void SendTcp(sf::Packet &Packet, sf::Uint32 Id);

        ReceiveStatus Receive(sf::Packet &Packet);

        bool IsActive();
        bool IsServer();
        bool IsClient();
    private:
        struct ClientPair
        {
            sf::Uint32 Id;
            sf::TcpSocket Tcp;
        };

        void CleanConns();

        bool Active;
        bool Server;

        sf::SocketSelector Selector;

        ClientPair ClientSocket;
        sf::TcpListener ServerTcp;
        sf::UdpSocket Udp;
        vector<ClientPair*> ServerConns;
};

#endif // NETWORKER_H
