#pragma once

#include <variant>
#include <memory>
#include <iostream>



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

class Service : public std::enable_shared_from_this<Service>
{
public:

    using ClientSocket = std::variant< std::shared_ptr<tcp::socket>, std::shared_ptr<ssl::stream<tcp::socket>> >;

    Service(bool& server_status, ClientManager& clientManager, ClientSocket clientSocket);

    void echo();

private:

    bool& m_server_status;
    ClientManager& m_clientManager;
    ClientSocket m_socket;

    net::streambuf m_buffer;
    std::vector<char> m_data;

};