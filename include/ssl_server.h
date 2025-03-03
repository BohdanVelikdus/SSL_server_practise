#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <deque>


#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

#include "boost/beast.hpp"

#include "Acceptor.h"
#include "Service.h"

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
namespace ssl = boost::asio::ssl;       
using tcp = boost::asio::ip::tcp;



class SSLServer
{
    SSLServer(const std::vector<tcp::endpoint>& endpoints, int threadCount = std::thread::hardware_concurrency());

    void changeStatus(bool status);

    void startServer();

    ~SSLServer();    

public:
    net::io_context m_ioc;    
    net::executor_work_guard<net::io_context::executor_type> m_executorWorkGuard;
    std::vector<std::unique_ptr<std::thread>> m_executionThreads;
    std::vector<std::shared_ptr<Acceptor>> m_acceptors;
    
    // pass to the acceptors
    std::deque<std::shared_ptr<Service>> m_clients;
    std::mutex m_mtx;
    ssl::context m_sslCtx;
    bool server_status = true;

    bool error = false;

};