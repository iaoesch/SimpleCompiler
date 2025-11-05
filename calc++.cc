#include <iostream>
#include "driver.hh"
#include "compact.h"

int
main (int argc, char *argv[])
{
  int res = 0;
  driver drv;
  for (int i = 1; i < argc; ++i)
    if (argv[i] == std::string ("-p"))
      drv.trace_parsing = true;
    else if (argv[i] == std::string ("-s"))
      drv.trace_scanning = true;
    else {
        do {
            drv.result.clear();
            if (drv.parse (argv[i]) != 0) {
                std::cout << "Abnormal parsing end" << std::endl;
            }
            if (drv.result.empty()) {
                std::cout << "<empty list>" << std::endl;
            }
            for (auto &s: drv.result) {
               s->Print(std::cout);
            }
        } while (argv[i] == std::string ("-"));
        std::cout << "argv = '" << argv[i] << "'" << std::endl;

    }
  return res;
}
