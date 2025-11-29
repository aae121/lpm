#pragma once

#include "process.hpp"
#include <vector>
#include <string>

// Process control operations
class ProcessController {
public:
    // Send signal to process
    static bool sendSignal(int pid, int signal);
    
    // Common signal operations
    static bool terminate(int pid);      // SIGTERM (15)
    static bool kill(int pid);           // SIGKILL (9)
    static bool stop(int pid);           // SIGSTOP (19)
    static bool resume(int pid);         // SIGCONT (18)
    static bool interrupt(int pid);      // SIGINT (2)
    
    // Change process priority
    static bool setPriority(int pid, int niceness);
    static int getPriority(int pid);
    
    // Get signal name from number
    static std::string getSignalName(int signal);
    
    // Get list of common signals
    static std::vector<std::pair<int, std::string>> getCommonSignals();
};
