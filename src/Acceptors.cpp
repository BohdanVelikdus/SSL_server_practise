#include "boost/asio.hpp"

#include <deque>
#include <memory>
#include <iostream>
#include <exception>

#include "Acceptors.h"
#include "Service.h"
#include "ClientManager.h"

#include <memory>

AcceptorEncrypt::AcceptorEncrypt(bool & error, net::io_context & ioc, tcp::endpoint ep, bool & status, ClientManager & clientManager, ssl::context & ctx) :
    m_ioc(ioc),
    m_ctx(ctx),
    m_serverStatus(status),
    m_clientManager(clientManager),
    m_acceptor(std::make_shared<tcp::acceptor>(ioc, ep))
{
    //boost::system::error_code ec;
    //(this->m_acceptor)->bind(ep, ec);
    //if(ec.value() != 0)
    //{
    //    std::cout << "Error with binding\n";
    //    error = true;
    //}
    //this->m_acceptor->open(tcp::v4(), ec);
    //if(ec.value() != 0)
    //{
    //    std::cout << "Error opening socket\n";
    //    error = true;
    //} 
}

void AcceptorEncrypt::start_accept()
{
    if(!this->m_serverStatus) return;
    
    auto self = shared_from_this();  
    std::shared_ptr<ssl::stream<tcp::socket>> socket = std::make_shared<ssl::stream<tcp::socket>>(this->m_ioc, this->m_ctx);
    m_acceptor->async_accept(socket->lowest_layer(), [this, self, socket](boost::system::error_code ec)
    {
        if(ec.value() == 0)
        {
            socket->async_handshake(ssl::stream_base::server, [this, self, socket](boost::system::error_code ec)
            {
                if (ec.value() == 0)
                {
                    // Create a new client in the ClientManager
                    std::shared_ptr<Service> service = std::make_shared<Service>(this->m_serverStatus, this->m_clientManager, socket);
                    this->m_clientManager.add_client(service);
                    //service-> start 
                }
                else
                {
                    std::cout << "Error happened: " << ec.message() << "\n";
                }
            });
        }
        else
        {
            std::cout << "Error happened: " << ec.message() << "\n";
        }
        this->start_accept();
    });
}


AcceptorPlain::AcceptorPlain(bool &error, net::io_context &ioc, tcp::endpoint ep, bool &status, ClientManager &clientManager) :
    m_ioc(ioc),
    m_serverStatus(status),
    m_clientManager(clientManager),
    m_acceptor(std::make_shared<tcp::acceptor>(ioc, ep))
{
    boost::system::error_code ec;
    (this->m_acceptor)->bind(ep, ec);
    if(ec.value() != 0)
    {
        std::cout << "Error with binding\n";
        error = true;
    }
    (this->m_acceptor)->open(tcp::v4(), ec);
    if(ec.value() != 0)
    {
        std::cout << "Error opening socket\n";
        error = true;
    } 
}

void AcceptorPlain::start_accept()
{
    if(!this->m_serverStatus) return;
    
    auto self = shared_from_this();  
    std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(this->m_ioc);
    m_acceptor->async_accept(socket->lowest_layer(), [this, self, socket](boost::system::error_code ec)
    {
        if(ec.value() == 0)
        {
            // add to the client manager a Service
            std::shared_ptr<Service> service = std::make_shared<Service>(this->m_serverStatus, this->m_clientManager, socket);
            this->m_clientManager.add_client(service);
            // service-> start something
        }
        else
        {
            std::cout << "Error happened: " << ec.message() << "\n";
        }
        this->start_accept();
    });
}
