#include "SSLServer.h"

#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <string>

#include <variant>

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"

#include "nlohmann/json.hpp"

#include "Utils.h"

SSLServer::SSLServer(const std::vector<std::pair<tcp::endpoint, bool>>& endpoints, int threadCount, std::string passwdCert, std::string certificatePath, std::string priKeyPath) : 
    m_executorWorkGuard(m_ioc.get_executor()),
    m_sslCtx(ssl::context::tls_server),
    m_executionThreads(threadCount)
{
    this->m_passwdCert = passwdCert;
    this->m_certificatePath = certificatePath;
    this->m_priKeyPath = priKeyPath;
    this->m_endpoints = endpoints;
    this->m_threadCount = threadCount;
}

bool SSLServer::initServer()
{
    boost::system::error_code ec;
    int i = 0;
    for(; i < m_threadCount; ++i)
    {
        m_executionThreads[i].reset(
            new std::thread([this]()
            {
                this->m_ioc.run();
            }));
    }

    this->m_sslCtx.set_options(
        ssl::context::default_workarounds | 
        ssl::context::no_sslv2 | 
        ssl::context::no_sslv3 | 
        ssl::context::no_tlsv1 |
        ssl::context::no_tlsv1_1 |
        ssl::context::single_dh_use 
    );

    try {
        this->m_sslCtx.set_password_callback(std::bind(&SSLServer::passwordCallback, this, std::placeholders::_1, std::placeholders::_2), ec);
    } catch (const std::exception& ex) {
        std::cout << "Error setting password callback: " << ex.what() << std::endl;
        return false;
    }
    this->m_sslCtx.use_certificate_chain_file(std::filesystem::absolute(this->m_certificatePath).string(), ec);
    if(ec.value() != 0)
    {
        std::cout << "Error with setting path to certificate\n";
        return false;
    }
    this->m_sslCtx.use_private_key_file(std::filesystem::absolute(this->m_priKeyPath).string(), ssl::context::pem, ec);
    if(ec.value() != 0)
    {
        std::cout << "Error with setting private key\n";
        return false;
    }

    // create a function to verify wht certificate and private key matches

    for(auto& [ep, encrypted] : m_endpoints)
    {
        if(encrypted)
        {
            std::optional<std::reference_wrapper<ssl::context>> sl = this->m_sslCtx;
            std::shared_ptr<Acceptor> acc = std::make_shared<Acceptor>(this->error, this->m_ioc, ep, sl, this->server_status, this->m_clientManager);
            this->m_acceptors.push_back(acc);
        }
        else
        {   
            std::shared_ptr<Acceptor> acc = std::make_shared<Acceptor>(this->error, this->m_ioc, ep, std::nullopt, this->server_status, this->m_clientManager);
            this->m_acceptors.push_back(acc);
        }
        
    }

    if(this->error)
        return false;
    
    return true;
}

bool SSLServer::getError()
{
    return this->error;
}

void SSLServer::changeStatus(bool status)
{
    this->server_status = status;
}

void SSLServer::startServer()
{
    if(!(this->error))
    {
        // here i will simply call a 
        for(auto& acceptor : m_acceptors)
        {
            acceptor->start_accept();
        }
    }
    else
    {
        std::cout << "Error with a server, check the console to view error\n";
        throw std::runtime_error("Error with server initialization");
    }
}

std::string SSLServer::passwordCallback(std::size_t max_length, ssl::context::password_purpose purpose)
{
    return this->m_passwdCert;
}

SSLServer::~SSLServer()
{
    this->server_status = false;
    this->m_ioc.stop();
    this->m_executorWorkGuard.reset();
    for(auto& thr : this->m_executionThreads)
    {
        if(thr->joinable())
            thr->join();
    }

}
