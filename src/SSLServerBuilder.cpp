#include "SSLServerBuilder.h"

#include "SSLServer.h"
#include <memory>

SSLServerBuilder SSLServerBuilder::createBuilder()
{
    return SSLServerBuilder();
}

SSLServerBuilder &SSLServerBuilder::setCertificatePath(std::string path)
{
    if(!isValidFile(path))
    {
        std::cout << "Error with path to certificate\n";
        return *this;
    }
    this->m_certificatePath = path;
    return *this;
}

SSLServerBuilder &SSLServerBuilder::setPrivateKeyPath(std::string path)
{
    if(!isValidFile(path))
    {
        std::cout << "Error with path to certificate\n";
        return *this;
    }
    this->m_priKeyPath = path;
    return *this;
}

SSLServerBuilder &SSLServerBuilder::setPassword(std::string passwd)
{
    this->m_passwdCert = passwd;
    return *this;
}

SSLServerBuilder &SSLServerBuilder::setEndpoints(const std::vector<tcp::endpoint> &endpoints)
{
    // verify the endpoints
    if(endpoints.size() == 0)
    {
        return *this;
    }
    net::io_context ioc;
    for(auto ep : endpoints)
    {
        tcp::acceptor acceptor(ioc, ep);
        boost::system::error_code ec;
        acceptor.open(ep.protocol(), ec);
        if (ec) {
            std::cout << "Failed to bind to endpoint " << ep << ": " << ec.message() << std::endl;
            return *this;
        }
    }
    this->endpoints = endpoints;
    return *this;   
}

SSLServerBuilder &SSLServerBuilder::setThreadCount(unsigned int num)
{
    if( num <= 0)
        return *this;
    this->threads = num;
    return *this;
}

std::unique_ptr<SSLServer> SSLServerBuilder::get()
{
    try
    {
        if(!(endpoints.has_value() && m_certificatePath.has_value() && m_priKeyPath.has_value()))
        {
            throw std::runtime_error("Not all necessary fields are init");
        }
        ptr_server = std::move(std::unique_ptr<SSLServer>(new SSLServer(endpoints.value(), threads, m_passwdCert.has_value() ? m_passwdCert.value() : std::string(""), m_certificatePath.value(), m_priKeyPath.value())));
        if(!ptr_server->initServer())
        {
            std::cout << "Error with initing server\n";
            throw std::runtime_error("Initing error");
        }
    }
    catch(boost::system::error_code ec)
    {
        std::cout << "Exception happend, msg: " << ec.message() << "\n";
        throw ;
    }
    return std::move(ptr_server);
}