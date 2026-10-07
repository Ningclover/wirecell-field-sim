#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"
#include <iostream>
#include <string>
#include <vector>

void check_events(std::string const &filename)
{
    using namespace art;
    std::vector<std::string> filenames(1, filename);
    int n = 0;
    std::cout << "=== Checking file: " << filename << " ===" << std::endl;
    for (gallery::Event ev(filenames); !ev.atEnd(); ev.next())
    {
        auto aux = ev.eventAuxiliary();
        std::cout << "Event " << n << ": run=" << aux.run()
                  << " event=" << aux.event() << std::endl;
        n++;
    }
    std::cout << "Total events: " << n << std::endl;
}
