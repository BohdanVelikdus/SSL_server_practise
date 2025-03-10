#include "ClientManager.h"

void ClientManager::add_client(std::shared_ptr<Service> clientToAdd)
{
    this->m_mtx.lock();
    this->m_clients.insert(clientToAdd);
    this->m_mtx.unlock();   
}

void ClientManager::remove_client(std::shared_ptr<Service> clientToRemove)
{
    this->m_mtx.lock();
    this->m_clients.erase(clientToRemove);
    this->m_mtx.unlock();  
}