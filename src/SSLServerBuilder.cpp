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

SSLServerBuilder& SSLServerBuilder::setEndpoints(const std::vector<ServerEndpoint>& endpoints)
{
    // Verify the endpoints
    if (endpoints.empty()) {
        return *this;
    }

    net::io_context ioc;
    for (const auto& ep : endpoints)
    {
        boost::system::error_code ec;
        tcp::acceptor acceptor(ioc, ep.endpoint);

        if(acceptor.is_open())
        {
            std::cout << "The socket is open successfully, closing...\n";
            acceptor.close();
        }
        else
        {
            std::cout << "The socket is really closed\n";
            return *this;
        }

    }
    // Store the endpoints
    this->endpoints = endpoints;
    return *this;
}


SSLServerBuilder &SSLServerBuilder::setThreadCount(unsigned int num)
{
    if( num <= 0 || num >= 32)
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
        if(!certificatesMatches(m_priKeyPath.value(), m_passwdCert.value_or(""), m_certificatePath.value()))
        {
            throw std::runtime_error("Certifiactes does not matches");
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