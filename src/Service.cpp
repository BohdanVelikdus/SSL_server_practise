#include "Service.h"

#include <memory>
#include <iostream>
#include <variant>
#include <string>

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"



Service::Service(bool &server_status, ClientManager &clientManager, ClientSocket clientSocket) :
    m_server_status(server_status),
    m_clientManager(clientManager),
    m_socket(std::move(clientSocket))
{
    
}

void Service::echo()
{
    std::visit([this](auto& socket)
    {
        auto self = shared_from_this();
        boost::asio::async_read(*(socket.get()), this->m_buffer, [this, self](boost::system::error_code ec, std::size_t bytes_received)
        {
            std::cout << "Get the data\n";
            std::istream istr(&(this->m_buffer));
            std::string str;
            std::getline(istr, str);
            std::cout << str << "\n";
        });
        
    }, this->m_socket);
}
