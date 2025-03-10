#include <iostream>
#include <csignal>
#include <vector>


#include "nlohmann/json.hpp"

#include "SSLServerBuilder.h"
#include "SSLServer.h"

#include "boost/asio.hpp"

using namespace std::literals;

int main()
{   
    std::string pathToKey = "/home/bohdan/repos/openssl_enc/11/intermediate/private/server.key";
    std::string pathToCert = "/home/bohdan/repos/openssl_enc/11/intermediate/certs/server.crt";

    std::vector<ServerEndpoint> eps;
    ServerEndpoint ep;
    ep.endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address_v4("127.0.0.1"), 50'001);
    ep.isEncrypted = true;
    eps.push_back(ep);
    //std::cout << ep.endpoint.address().to_string() << "\n";
    try
    {
        auto server = SSLServerBuilder::createBuilder().setCertificatePath(pathToCert).setPrivateKeyPath(pathToKey).setPassword("1234").setEndpoints(eps).get();
        server->startServer();
        std::cout << "End, waiting...\n";
        int num;
        std::cin >> num;
        std::this_thread::sleep_for(1000s);
    }
    catch(boost::system::error_code ec)
    {
        std::cout << "Error caught: " << ec.message() << "\n";
    }
    catch(std::runtime_error re)
    {
        std::cout << "Runtime error: " << re.what() << "\n";
    }

    return 0;
}