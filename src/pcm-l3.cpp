#include <iostream>
#include <vector>
#include "cpucounters.h"

#define CORE_TO_MONITOR 5
#define POLL_INTERVAL 1 // seconds

using namespace pcm;

int main() {
    PCM *m = PCM::getInstance();

    PCM::ErrorCode status = m->program();
    if (status != PCM::Success) {
        std::cerr << "Error programming PCM: " << status << std::endl;
        if (status == PCM::MSRAccessDenied) {
            std::cerr << "Access to MSRs denied. Try running with elevated privileges." << std::endl;
        }
        return -1;
    }

    // Check if the specified core is within the valid range
    if (CORE_TO_MONITOR >= m->getNumCores()) {
        std::cerr << "Core " << CORE_TO_MONITOR << " is out of the range of available cores." << std::endl;
        return -2;
    }

    // Check if L3 Cache Misses can be monitored
    if (!m->isL3CacheMissesAvailable()) {
        std::cerr << "L3 cache misses not supported on this processor." << std::endl;
        return -3;
    }

    while (1) {
        std::vector<CoreCounterState> cstates1, cstates2;
        std::vector<SocketCounterState> sktstate1, sktstate2;
        SystemCounterState sstate1, sstate2;

        // Get initial state
        m->getAllCounterStates(sstate1, sktstate1, cstates1);

        // Wait for the polling interval
        sleep(POLL_INTERVAL);

        // Get new state after interval
        m->getAllCounterStates(sstate2, sktstate2, cstates2);

        // Calculate L3 cache misses for the specific core
        uint64 l3CacheMisses = getL3CacheMisses(cstates1[CORE_TO_MONITOR], cstates2[CORE_TO_MONITOR]);

        std::cout << "L3 Cache Misses on Core " << CORE_TO_MONITOR << ": " << l3CacheMisses << std::endl;
    }

    return 0;
}