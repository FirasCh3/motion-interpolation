#include <SFML/Network.hpp>
#include <iostream>
#ifndef CLIENT_NETWORKCLIENT_H
#define CLIENT_NETWORKCLIENT_H
using namespace sf;
using namespace std;
class NetworkClient {
public:
    NetworkClient():port_(Socket::AnyPort), ip_(IpAddress(127, 0, 0, 1)){};

    int port() const {
        return port_;
    }

    IpAddress ip() const {
        return ip_;
    }

    void send_data() {
        UdpSocket::Status statuts = socket_.bind(port_, ip_);
        if (statuts!=Socket::Status::Done) {
            cerr<<"Failed to initialize client"<<endl;
        }
        IpAddress server_ip = IpAddress(127, 0, 0, 1);
        Packet data_to_send = Packet();
        data_to_send.operator<<("hello");
        if (socket_.send(data_to_send, server_ip, 54000)!=Socket::Status::Done) {
            cerr<<"Failed to send data to server"<<endl;
        }

    }

private:
    int port_;
    IpAddress ip_;
    UdpSocket socket_;
    TcpListener listener_;
};
#endif //CLIENT_NETWORKCLIENT_H
