#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP


#include <iostream>
class StopRequested {

};

namespace yy {
class location;
}
typedef yy::location LocationType;

class Environment {
public:
    enum class Color{Default, Black, Red, Green, Blue};
    bool DoEvaluateFunctions = true;

private:
    Color CurrentColor = Color::Default;
    std::vector<Color> PreviousColors;

public:
    virtual ~Environment();
    virtual std::ostream &OutputStream();
    virtual std::istream &InputStream();
    virtual std::ostream &DebugOutput();
    virtual void Tracing(LocationType const &Loc, std::string Statement);
    virtual void ExecutionStarted();
    virtual void ExecutionStopped();
    void         ThrowIfStoppRequested() {if (CheckForStop()){throw StopRequested();}}
    void SetOutputStreamColor(Color Col);// << "\x1b[31m"
    void RestoreOutputStreamColor();

private:
    virtual bool CheckForStop();
    void murks();
    void SetOutputStreamColorTo(Color Col);
};

class ErrorEnvironment : public Environment {


    // Environment interface
public:
    virtual std::ostream &OutputStream() override;
    virtual std::istream &InputStream() override;
    virtual std::ostream &DebugOutput() override;
    virtual void Tracing(const LocationType &Loc, std::string Statement) override;
    virtual void ExecutionStarted() override;
    virtual void ExecutionStopped() override;

private:
    virtual bool CheckForStop() override;
};




#endif // ENVIRONMENT_HPP
