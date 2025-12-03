#include "core/Client.h"
#include "db/MySQLManager.h"

Client::Client(const int fd) : m_fd(fd) 
{
}

Client::~Client() 
{

}