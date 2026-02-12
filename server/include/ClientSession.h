#pragma once

#include <SFML/Network.hpp>

class ClientSession {
private:
    int id;
    sf::IpAddress address;
    unsigned short port;

public:
    ClientSession(int id,
                  const sf::IpAddress& address,
                  unsigned short port);

    int getId() const;

    const sf::IpAddress& getAddress() const;
    unsigned short getPort() const;

    bool matches(const sf::IpAddress& address,
                 unsigned short port) const;
};
