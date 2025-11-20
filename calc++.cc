#include <iostream>
#include "driver.hh"
#include "compact.h"

int
drivermain (int argc, char *argv[])
{
    int res = 0;
    Environment Env;
    driver drv(Env);
    for (int i = 1; i < argc; ++i)
        if (argv[i] == std::string ("-p"))
            drv.trace_parsing = true;
        else if (argv[i] == std::string ("-s"))
            drv.trace_scanning = true;
        else {
            drv.trace_parsing = true;
            do {
                drv.result.clear();
                try {
                    if (drv.parse (argv[i]) != 0) {
                        std::cout << "Abnormal parsing end" << std::endl;
                    }
                } catch (ErrorBaseClass &e) {
                    std::cout << "Exception: " << e.what() << std::endl;
                    std::cout << drv.location << std::endl;
                } catch (std::exception &e) {
                    std::cout << "Exception: " << e.what() << std::endl;
                    std::cout << drv.location << std::endl;
                } catch (...) {
                    std::cout << "Unknown Exception: " << std::endl;
                    std::cout << drv.location << std::endl;

                }

                if (drv.result.empty()) {
                    std::cout << "<empty list>" << std::endl;
                }
                for (auto &s: drv.result) {
                    s->Print(std::cout);
                }
                drv.Variables.Dump(std::cout);
            } while (argv[i] == std::string ("-"));
            std::cout << "argv = '" << argv[i] << "'" << std::endl;

        }
    return res;
}
