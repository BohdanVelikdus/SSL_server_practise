#include "boost/asio.hpp"

#include <deque>
#include <memory>
#include <iostream>
#include <exception>

#include "Acceptor.h"
#include "Service.h"

#include <memory>

Acceptor::Acceptor(bool& error, net::io_context &ioc, tcp::endpoint ep, ssl::context &sslCtx, bool &status, std::deque<std::shared_ptr<Service>> &deque, std::mutex &mtx) :
    m_ioc(ioc), 
    m_acceptor(ioc), 
    m_serverStatus(status), 
    m_clients(deque), 
    m_mtx(mtx),
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
    if(this->m_serverStatus)
    {
        auto socket = std::make_shared<ssl::stream<tcp::socket>>(this->m_ioc, this->m_sslCtx);
        m_acceptor.async_accept(socket->lowest_layer(), [this, socket](boost::system::error_code ec, tcp::socket Socket)
        {
            if(ec.value() == 0)
            {
                // means the client successfully connected
                this->m_mtx.lock();

                

                this->m_mtx.unlock();
            }
            this->start_accept();
        });
    }
}
