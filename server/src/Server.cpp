#include "Server.h"
#include <iostream>
using namespace std;

Server::Server(unsigned short server_port)
    : server_port(server_port), is_running(false) {}

void Server::start() {
    if (server_socket.bind(server_port) != sf::Socket::Status::Done) {
        cerr << "Failed to bind server socket\n";
        return;
    }

    server_socket.setBlocking(true);
    is_running = true;

    cout << "Server running on port " << server_port << "\n";

    while (is_running) {
        sf::Packet packet;
        std::optional<sf::IpAddress> sender;
        unsigned short sender_port;

        auto status = server_socket.receive(packet, sender, sender_port);

        if (status == sf::Socket::Status::Done && sender.has_value()) {
            handle_packet(packet, sender.value(), sender_port);
        }
    }
}

void Server::stop() {
    is_running = false;
    server_socket.unbind();
}

void Server::handle_packet(sf::Packet& packet,
                           const sf::IpAddress& sender,
                           unsigned short sender_port) {

    register_client(sender, sender_port);
    forward_to_others(packet, sender, sender_port);
}

void Server::register_client(const sf::IpAddress& sender,
                             unsigned short sender_port) {

    std::lock_guard<std::mutex> lock(clients_mutex);

    for (const auto& client : clients)
        if (client.matches(sender, sender_port))
            return;

    int new_id = clients.size() + 1;
    clients.emplace_back(new_id, sender, sender_port);

    cout << "Registered client "
         << new_id
         << " ("
         << sender
         << ":"
         << sender_port
         << ")\n";
}

void Server::forward_to_others(sf::Packet& packet,
                               const sf::IpAddress& sender,
                               unsigned short sender_port) {

    std::lock_guard<std::mutex> lock(clients_mutex);

    for (const auto& client : clients) {
        if (!client.matches(sender, sender_port)) {
            auto status = server_socket.send(packet,
                                             client.get_address(),
                                             client.get_port());
            if (status != sf::Socket::Status::Done) {
                cerr << "Failed to send packet to client "
                     << client.get_id() << "\n";
            }
        }
    }
}
