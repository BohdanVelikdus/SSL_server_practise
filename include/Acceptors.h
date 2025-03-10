#pragma once

#include <deque>
#include <mutex>
#include <memory>
#include <unordered_set>

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

#include "boost/beast.hpp"

#include "ClientManager.h"

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
namespace ssl = boost::asio::ssl;       
using tcp = boost::asio::ip::tcp;

class Policy
{   
public:
    virtual void start_accept() = 0;
};

class AcceptorEncrypt : public std::enable_shared_from_this<AcceptorEncrypt>, public Policy
{
public:

    AcceptorEncrypt(bool& error, net::io_context& ioc, tcp::endpoint ep, bool& status, ClientManager& clientManager, ssl::context& ctx);

    void start_accept() override;

private:
    net::io_context& m_ioc;
    std::shared_ptr<tcp::acceptor> m_acceptor;

    ssl::context& m_ctx;

    // reference type from the server
    bool& m_serverStatus;
    ClientManager& m_clientManager;
};


class AcceptorPlain : public std::enable_shared_from_this<AcceptorPlain>, public Policy
{
public:

    AcceptorPlain(bool& error, net::io_context& ioc, tcp::endpoint ep, bool& status, ClientManager& clientManager);

    void start_accept() override;

private:
    net::io_context& m_ioc;
    std::shared_ptr<tcp::acceptor> m_acceptor;

    // reference type from the server
    bool& m_serverStatus;
    ClientManager& m_clientManager;

};

