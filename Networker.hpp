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
        SurfaceUpdate
    };
}
typedef PacketTypes::PacketType PacketType;

sf::Packet& operator>> (sf::Packet &Packet, PacketType &Type);
sf::Packet& operator<< (sf::Packet &Packet, PacketType &Type);

/** General packets
sf::Uint32 SourceId, PacketType Type
**/
/** Packets
--ConnectResponse--
sf::Uint32 Id, sf::Uint32 UdpPort
--SurfaceUpdate--
string Name
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
