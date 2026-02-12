#include "ClientSession.h"

ClientSession::ClientSession(int id,
                             const sf::IpAddress& address,
                             unsigned short port)
    : id(id), address(address), port(port) {}

int ClientSession::getId() const {
    return id;
}

const sf::IpAddress& ClientSession::getAddress() const {
    return address;
}

unsigned short ClientSession::getPort() const {
    return port;
}

bool ClientSession::matches(const sf::IpAddress& addr,
                            unsigned short p) const {
    return address == addr && port == p;
}
