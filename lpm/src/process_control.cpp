#include "process_control.hpp"
#include <signal.h>
#include <sys/resource.h>
#include <unistd.h>
#include <cerrno>

// Send signal to process
bool ProcessController::sendSignal(int pid, int signal) {
    if (pid <= 0) {
        return false;
    }
    return (::kill(pid, signal) == 0);
}

// Terminate process gracefully (SIGTERM)
bool ProcessController::terminate(int pid) {
    return sendSignal(pid, SIGTERM);
}

// Force kill process (SIGKILL)
bool ProcessController::kill(int pid) {
    return sendSignal(pid, SIGKILL);
}

// Stop/suspend process (SIGSTOP)
bool ProcessController::stop(int pid) {
    return sendSignal(pid, SIGSTOP);
}

// Resume stopped process (SIGCONT)
bool ProcessController::resume(int pid) {
    return sendSignal(pid, SIGCONT);
}

// Send interrupt signal (SIGINT)
bool ProcessController::interrupt(int pid) {
    return sendSignal(pid, SIGINT);
}

// Set process priority (nice value: -20 to 19, lower = higher priority)
bool ProcessController::setPriority(int pid, int niceness) {
    if (pid <= 0 || niceness < -20 || niceness > 19) {
        return false;
    }
    return (setpriority(PRIO_PROCESS, pid, niceness) == 0);
}

// Get process priority
int ProcessController::getPriority(int pid) {
    if (pid <= 0) {
        return 0;
    }
    
    errno = 0;
    int priority = getpriority(PRIO_PROCESS, pid);
    
    if (errno != 0) {
        return 0;
    }
    
    return priority;
}

// Get signal name from number
std::string ProcessController::getSignalName(int signal) {
    switch (signal) {
        case SIGHUP:    return "SIGHUP";
        case SIGINT:    return "SIGINT";
        case SIGQUIT:   return "SIGQUIT";
        case SIGILL:    return "SIGILL";
        case SIGTRAP:   return "SIGTRAP";
        case SIGABRT:   return "SIGABRT";
        case SIGBUS:    return "SIGBUS";
        case SIGFPE:    return "SIGFPE";
        case SIGKILL:   return "SIGKILL";
        case SIGUSR1:   return "SIGUSR1";
        case SIGSEGV:   return "SIGSEGV";
        case SIGUSR2:   return "SIGUSR2";
        case SIGPIPE:   return "SIGPIPE";
        case SIGALRM:   return "SIGALRM";
        case SIGTERM:   return "SIGTERM";
        case SIGCHLD:   return "SIGCHLD";
        case SIGCONT:   return "SIGCONT";
        case SIGSTOP:   return "SIGSTOP";
        case SIGTSTP:   return "SIGTSTP";
        case SIGTTIN:   return "SIGTTIN";
        case SIGTTOU:   return "SIGTTOU";
        default:        return "UNKNOWN";
    }
}

// Get list of common signals for user selection
std::vector<std::pair<int, std::string>> ProcessController::getCommonSignals() {
    return {
        {SIGTERM, "SIGTERM (15) - Terminate gracefully"},
        {SIGKILL, "SIGKILL (9) - Force kill"},
        {SIGINT,  "SIGINT (2) - Interrupt"},
        {SIGHUP,  "SIGHUP (1) - Hangup"},
        {SIGSTOP, "SIGSTOP (19) - Stop process"},
        {SIGCONT, "SIGCONT (18) - Continue process"},
        {SIGUSR1, "SIGUSR1 (10) - User signal 1"},
        {SIGUSR2, "SIGUSR2 (12) - User signal 2"}
    };
}
