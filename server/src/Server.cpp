#include "Server.h"
#include <iostream>
using namespace std;

Server::Server(unsigned short port)
    : port(port), running(false) {}

void Server::start() {
    if (socket.bind(port) != sf::Socket::Status::Done) {
        cerr << "Failed to bind server socket\n";
        return;
    }

    socket.setBlocking(true);
    running = true;

    cout << "Server running on port " << port << "\n";

    while (running) {
        sf::Packet packet;
        std::optional<sf::IpAddress> sender;
        unsigned short senderPort;

        auto status = socket.receive(packet, sender, senderPort);

        if (status == sf::Socket::Status::Done && sender.has_value()) {
            handlePacket(packet, sender.value(), senderPort);
        }
    }
}

void Server::stop() {
    running = false;
    socket.unbind();
}

void Server::handlePacket(sf::Packet& packet,
                          const sf::IpAddress& sender,
                          unsigned short senderPort) {

    registerClient(sender, senderPort);
    forwardToOthers(packet, sender, senderPort);
}

void Server::registerClient(const sf::IpAddress& sender,
                            unsigned short senderPort) {

    std::lock_guard<std::mutex> lock(clientsMutex);

    for (const auto& client : clients)
        if (client.matches(sender, senderPort))
            return;

    int newId = clients.size() + 1;
    clients.emplace_back(newId, sender, senderPort);

    cout << "Registered client "
              << newId
              << " ("
              << sender
              << ":"
              << senderPort
              << ")\n";
}

void Server::forwardToOthers(sf::Packet& packet,
                             const sf::IpAddress& sender,
                             unsigned short senderPort) {

    std::lock_guard<std::mutex> lock(clientsMutex);

    for (const auto& client : clients) {
    if (!client.matches(sender, senderPort)) {
        auto status = socket.send(packet,
                                  client.getAddress(),
                                  client.getPort());
        if (status != sf::Socket::Status::Done) {
            cerr << "Failed to send packet to client "
                      << client.getId() << "\n";
        }
        }
    }
}
