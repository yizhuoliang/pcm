#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sched.h>
#include "cpucounters.h"

#define POLL_INTERVAL 1 // seconds

using namespace pcm;

int main() {
    PCM *m = PCM::getInstance();

    PCM::ErrorCode status = m->program();
    if (status != PCM::Success) {
        std::cerr << "Error programming PCM: " << status << std::endl;
        return -1;
    }

    // Pinning the process to core 0
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0) {
        std::cerr << "Failed to pin the process to core 0." << std::endl;
        return -2;
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = startTime + std::chrono::seconds(POLL_INTERVAL);
    int iterations = 0;

    while (std::chrono::high_resolution_clock::now() < endTime) {
        std::vector<CoreCounterState> cstates;
        m->getCoreCounterStates(cstates); // Call the measurement function
        iterations++;
    }

    std::cout << "Iterations completed in 1 second: " << iterations << std::endl;

    return 0;
}
