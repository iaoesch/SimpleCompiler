
#include "environment.hpp"
#include "driver.hh"

/* Ansi Codes

    fg  bg
    30 	40 	Black
    31 	41 	Red
    32 	42 	Green
    33 	43 	Yellow
    34 	44 	Blue
    35 	45 	Magenta
    36 	46 	Cyan
    37 	47 	White
*/

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

void Environment::Tracing(const LocationType &Loc, std::string Statement)
{
    SetOutputStreamColor(Color::Blue);
    std::cout << "[" << Loc << "]" << Statement << std::endl;
    RestoreOutputStreamColor();
}

void Environment::SetOutputStreamColor(Color Col)
{
    PreviousColors.push_back(CurrentColor);
    SetOutputStreamColorTo(Col);
}

void Environment::RestoreOutputStreamColor()
{
    if (PreviousColors.empty()) {
        SetOutputStreamColorTo(Color::Default);
    } else {
        SetOutputStreamColorTo(PreviousColors.back());
        PreviousColors.pop_back();
    }
}


void Environment::SetOutputStreamColorTo(Color Col)
{
    CurrentColor = Col;
    switch(Col) {

    case Color::Default: OutputStream() << "\x1b[0m"; break;
    case Color::Black:   OutputStream() << "\x1b[30m"; break;
    case Color::Red:     OutputStream() << "\x1b[31m"; break;
    case Color::Green:   OutputStream() << "\x1b[32m"; break;
    case Color::Blue:    OutputStream() << "\x1b[34m"; break;
    default:    break;
    }
}



