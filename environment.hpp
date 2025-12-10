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

private:
    Color CurrentColor = Color::Default;
    std::vector<Color> PreviousColors;

public:
    virtual ~Environment();
    virtual std::ostream &OutputStream();
    virtual std::istream &InputStream();
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






#endif // ENVIRONMENT_HPP
