#include "ssl_server.h"


#include <iostream>
#include <memory>
#include <vector>
#include <thread>

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

#include "boost/beast.hpp"

#include "nlohmann/json.hpp"


SSLServer::SSLServer(const std::vector<tcp::endpoint>& endpoints, int threadCount) : 
    m_executorWorkGuard(m_ioc.get_executor()),
    m_sslCtx(ssl::context::tls_server)
{
    int i = 0;
    for(; i < threadCount; ++i)
    {
        m_executionThreads[i].reset(
            new std::thread([this]()
            {
                this->m_ioc.run();
            }));
    }

    for(auto& ep : endpoints)
    {
        std::shared_ptr<Acceptor> acc = std::make_shared<Acceptor>(this->error, this->m_ioc, ep, this->m_sslCtx, this->server_status, this->m_clients, this->m_mtx);
        this->m_acceptors.push_back(acc);
    }
}

void SSLServer::changeStatus(bool status)
{
    this->server_status = status;
}

void SSLServer::startServer()
{
    if(!(this->error))
    {
        // here i will simply call a 
        for(auto& acceptor : m_acceptors)
        {
            acceptor->start_accept();
        }
    }
    std::cout << "Error with a server, check the console to view error\n";
    throw std::runtime_error("Error with server initialization");
}

SSLServer::~SSLServer()
{
    this->server_status = false;
    this->m_executorWorkGuard.reset();
    this->m_ioc.stop();
    for(auto& thr : this->m_executionThreads)
    {
        if(thr->joinable())
            thr->join();
    }
    
}
