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

class Acceptor : public std::enable_shared_from_this<Acceptor>
{
public:
    Acceptor(bool& error, net::io_context& ioc, tcp::endpoint ep, std::optional<std::reference_wrapper<ssl::context>> sslCtx, bool& status, ClientManager& clientManager);

    void start_accept();

private:
    net::io_context& m_ioc;
    tcp::acceptor m_acceptor;

    // reference type from the server
    bool& m_serverStatus;
    ClientManager& m_clientManager;
    std::optional<std::reference_wrapper<ssl::context>> m_sslCtx;
};