#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <unordered_set>
#include <exception>
#include <string>
#include <utility>

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

#include "boost/beast.hpp"

#include "Acceptor.h"
#include "Service.h"
#include "ClientManager.h"
#include "Utils.h"


namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
namespace ssl = boost::asio::ssl;       
using tcp = boost::asio::ip::tcp;

class SSLServer
{
private:
    friend class SSLServerBuilder;
public:
    void changeStatus(bool status);

    void startServer();

    bool initServer();

    ~SSLServer();    

private:

    std::string passwordCallback(std::size_t max_length, ssl::context::password_purpose purpose);

    SSLServer(const std::vector<std::pair<tcp::endpoint, bool>>& endpoints, int threadCount, std::string passwdCert, std::string certificatePath, std::string priKeyPath );

    net::io_context m_ioc;    
    net::executor_work_guard<net::io_context::executor_type> m_executorWorkGuard;
    std::vector<std::unique_ptr<std::thread>> m_executionThreads;
    std::vector<std::shared_ptr<Acceptor>> m_acceptors;
    
    std::string m_passwdCert;
    std::string m_certificatePath;
    std::string m_priKeyPath;
    std::vector<std::pair<tcp::endpoint, bool>> m_endpoints;
    int m_threadCount;

    bool getError();

    // pass to the acceptors
    ssl::context m_sslCtx;
    bool server_status = true;
    ClientManager m_clientManager;

    bool error = false;

};