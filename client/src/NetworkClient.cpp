#include "NetworkClient.h"
#include "Player.h"
#include <iostream>
#include <SFML/Network.hpp>
using namespace std;
using namespace sf;
NetworkClient::NetworkClient():port_(Socket::AnyPort), ip_(IpAddress(127, 0, 0, 1)), server_ip_(IpAddress(127, 0, 0, 1)), server_port_(54000)
{
      socket_.setBlocking(false);
}
int NetworkClient::port() {
      return port_;
}

IpAddress NetworkClient::ip() {
      return ip_;
}

void NetworkClient::send_data(Vector2f position) {
      Packet data_to_send = Packet();
      data_to_send.operator<<(position.x)<<position.y;
      if (socket_.send(data_to_send, server_ip_, 54000)!=Socket::Status::Done) {
            cerr<<"Failed to send data to server"<<endl;
      }

}
void NetworkClient::connect()
{
      cout<<"yo"<<endl;
      UdpSocket::Status statuts = socket_.bind(port_, ip_);
      if (statuts!=Socket::Status::Done) {
            cerr<<"Failed to initialize client"<<endl;
      }
}
void NetworkClient::receive_data(Player &remote_player)
{

      Packet packet;
      optional<IpAddress> sender_address;
      unsigned short sender_port;
      if (socket_.receive(packet, sender_address, sender_port)==Socket::Status::Done)
      {

            if (sender_address.has_value())
            {
                  float x = 0.f,y = 0.f;
                  packet.operator>>(x)>>y;
                  remote_player.moveRemotePlayer(x, y);
            }

      }
}