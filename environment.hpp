#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP


#include <iostream>

class Environment {
public:
    std::ostream &OutputStream() {return std::cout;}
    std::istream &InputStream() {return std::cin;}     
};

#endif // ENVIRONMENT_HPP
