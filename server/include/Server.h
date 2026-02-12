#pragma once

#include <vector>
#include <atomic>
#include <mutex>
#include <SFML/Network.hpp>
#include "ClientSession.h"

class Server {
private:
    unsigned short port;
    sf::UdpSocket socket;

    std::vector<ClientSession> clients;
    std::atomic<bool> running;
    std::mutex clientsMutex;

public:
    explicit Server(unsigned short port);

    void start();
    void stop();

private:
    void handlePacket(sf::Packet& packet,
                      const sf::IpAddress& sender,
                      unsigned short senderPort);

    void registerClient(const sf::IpAddress& sender,
                        unsigned short senderPort);

    void forwardToOthers(sf::Packet& packet,
                         const sf::IpAddress& sender,
                         unsigned short senderPort);
};
