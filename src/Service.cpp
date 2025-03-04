#include "Service.h"

Service::Service(bool &server_status, ClientManager &clientManager, ssl::stream<tcp::socket> socket):
    m_server_status(server_status),
    m_clientManager(clientManager),
    m_socket(std::move(socket))
{
    
}