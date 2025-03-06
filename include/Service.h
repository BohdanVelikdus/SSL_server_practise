#pragma once

#include <variant>
#include <memory>

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"

#include "Service.h"
#include "ClientManager.h"

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
namespace ssl = boost::asio::ssl;       
using tcp = boost::asio::ip::tcp;

class ClientManager;

class Service
{
public:
    
    Service(bool& server_status, ClientManager& clientManager, ssl::stream<tcp::socket> socket);

private:

    bool& m_server_status;
    ClientManager& m_clientManager;
    std::variant<ssl::stream<tcp::socket>, tcp::socket> m_socket;

};