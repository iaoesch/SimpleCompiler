#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP


#include <iostream>
class StopRequested {

};

class Environment {
public:
    virtual ~Environment();
    virtual std::ostream &OutputStream();
    virtual std::istream &InputStream();
    virtual void ExecutionStarted();
    virtual void ExecutionStopped();
    void         ThrowIfStoppRequested() {if (CheckForStop()){throw StopRequested();}}

private:
    virtual bool CheckForStop();
    void murks();
};





#endif // ENVIRONMENT_HPP
