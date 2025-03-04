#pragma once

#include <unordered_set>
#include <thread>
#include <memory>
#include <iostream>

#include "Service.h"

#include "boost/beast.hpp"

class Service;


class ClientManager
{
public:
    ClientManager() = default;

    void add_client();

    void remove_client(std::shared_ptr<Service> clientToRemove);

    ~ClientManager() = default;

private:
    std::unordered_set<std::shared_ptr<Service>> m_clients;
    std::mutex m_mtx;

};