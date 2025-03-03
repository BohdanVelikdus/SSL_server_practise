#pragma once

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
namespace ssl = boost::asio::ssl;       
using tcp = boost::asio::ip::tcp;

class Service
{
public:
    Service();


private:

};