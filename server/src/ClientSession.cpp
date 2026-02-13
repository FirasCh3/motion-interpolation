#include "ClientSession.h"

ClientSession::ClientSession(int client_id,
                             const sf::IpAddress& client_address,
                             unsigned short client_port)
    : client_id(client_id), client_address(client_address), client_port(client_port) {}

int ClientSession::get_id() const {
    return client_id;
}

const sf::IpAddress& ClientSession::get_address() const {
    return client_address;
}

unsigned short ClientSession::get_port() const {
    return client_port;
}

bool ClientSession::matches(const sf::IpAddress& address,
                            unsigned short port) const {
    return client_address == address && client_port == port;
}
