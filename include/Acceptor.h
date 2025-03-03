#pragma once

#include <deque>
#include <mutex>
#include <memory>

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

#include "boost/beast.hpp"

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
namespace ssl = boost::asio::ssl;       
using tcp = boost::asio::ip::tcp;


class Acceptor : public std::enable_shared_from_this<Acceptor>
{
public:
    Acceptor(bool& error, net::io_context& ioc, tcp::endpoint ep, ssl::context& sslCtx, bool& status, std::deque<std::shared_ptr<Service>>& deque, std::mutex& mtx);

    void start_accept();

private:

    net::io_context& m_ioc;
    tcp::acceptor m_acceptor;

    bool& m_serverStatus;
    std::deque<std::shared_ptr<Service>>& m_clients; 
    std::mutex& m_mtx;
    ssl::context& m_sslCtx;

};