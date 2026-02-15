#ifndef CLIENT_NETWORKCLIENT_H
#define CLIENT_NETWORKCLIENT_H
#include <SFML/Network.hpp>
class Player;
using namespace sf;
using namespace std;
class NetworkClient {
public:
    NetworkClient();
    int port();
    IpAddress ip();
    void send_data(Vector2f position);
    void connect();
    void receive_data(Player &remote_player);



private:
    int port_;
    IpAddress ip_;
    UdpSocket socket_;
    TcpListener listener_;
    IpAddress server_ip_;
    unsigned short server_port_;
};
#endif //CLIENT_NETWORKCLIENT_H
