#pragma once

#include <SFML/Network.hpp>

class ClientSession {
private:
    int client_id;
    sf::IpAddress client_address;
    unsigned short client_port;

public:
    ClientSession(int client_id,
                  const sf::IpAddress& client_address,
                  unsigned short client_port);

    int get_id() const;

    const sf::IpAddress& get_address() const;
    unsigned short get_port() const;

    bool matches(const sf::IpAddress& address,
                 unsigned short port) const;
};
