#include "driver.hh"
#include "parser.hpp"

driver::driver ()
  : trace_parsing (false), trace_scanning (false)
{
//  variables["one"] = 1;
//  variables["two"] = 2;
}

int
driver::parse (const std::string &f)
{
  file = f;
  location.initialize (&file);
  scan_begin ();
  yy::parser parser (*this);
  parser.set_debug_level (trace_parsing);
  int res = 0;
  res = parser.parse ();
  scan_end ();
  return res;
}

void driver::halt()
{
    std::cout << "Error happened" << std::endl;
}
