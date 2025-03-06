#include "boost/asio.hpp"

#include <deque>
#include <memory>
#include <iostream>
#include <exception>

#include "Acceptor.h"
#include "Service.h"
#include "ClientManager.h"

#include <memory>

Acceptor::Acceptor(bool& error, net::io_context& ioc, tcp::endpoint ep, std::optional<std::reference_wrapper<ssl::context>> sslCtx, bool& status, ClientManager& clientManager) : 
    m_ioc(ioc), 
    m_acceptor(ioc), 
    m_serverStatus(status), 
    m_clientManager(clientManager),
    m_sslCtx(sslCtx)
{
    boost::system::error_code ec;
    this->m_acceptor.bind(ep, ec);
    if(ec.value() != 0)
    {
        std::cout << "Error with binding\n";
        error = true;
    }
    this->m_acceptor.open(tcp::v4(), ec);
    if(ec.value() != 0)
    {
        std::cout << "Error opening socket\n";
        error = true;
    }
}

void Acceptor::start_accept()
{
    if(!this->m_serverStatus) return;
    
    auto self = shared_from_this();
    if(this->m_sslCtx.has_value())
    {
        // means it has a ssl context
        auto socket = std::make_shared<ssl::stream<tcp::socket>>(this->m_ioc, this->m_sslCtx.value());
        m_acceptor.async_accept(socket->lowest_layer(), [this, self, socket](boost::system::error_code ec)
        {
            if(ec.value() == 0)
            {
                // means the client successfully connected

            }
            else
            {
                std::cout << "Error happened: " << ec.message() << "\n";
            }
            this->start_accept();
        });
    }
    else
    {
        // means the communication is not encrypted
        auto socket = std::make_shared<tcp::socket>(this->m_ioc);
        m_acceptor.async_accept(*(socket.get()), [this, self, socket](boost::system::error_code ec)
        {
            if(ec.value() == 0)
            {
                // means the client successfully connected
                auto service = std::make_shared<Service>();
                
            }
            else
            {
                std::cout << "Error happened: " << ec.message() << "\n";
            }
            this->start_accept();
        });
    }
    
}
