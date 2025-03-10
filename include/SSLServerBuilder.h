#pragma once

#include <optional>
#include <memory>
#include <iostream>
#include <utility>


#include "Utils.h"
#include "SSLServer.h"

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

#include "boost/beast.hpp"


namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
namespace ssl = boost::asio::ssl;       
using tcp = boost::asio::ip::tcp;

class SSLServerBuilder
{
public:

    static SSLServerBuilder createBuilder();

    SSLServerBuilder& setCertificatePath(std::string path);

    SSLServerBuilder& setPrivateKeyPath(std::string path);

    SSLServerBuilder& setPassword(std::string passwd);

    SSLServerBuilder& setEndpoints(const std::vector<ServerEndpoint>& endpoints);

    SSLServerBuilder& setThreadCount(unsigned int num);

    std::unique_ptr<SSLServer> get();

private:

    SSLServerBuilder() = default;

    std::unique_ptr<SSLServer> ptr_server = nullptr;
    int threads = std::thread::hardware_concurrency();
    std::optional<std::vector<ServerEndpoint>> endpoints = std::nullopt;
    std::optional<std::string> m_passwdCert = std::nullopt;
    std::optional<std::string> m_certificatePath = std::nullopt;
    std::optional<std::string> m_priKeyPath = std::nullopt;
};
