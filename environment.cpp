
#include "environment.hpp"

bool Environment::CheckForStop()
{
    return false;
}



Environment::~Environment()
{

}

std::ostream &Environment::OutputStream() {return std::cout;}

std::istream &Environment::InputStream() {return std::cin;}

void Environment::ExecutionStarted()
{

}
void Environment::ExecutionStopped()
{

}
