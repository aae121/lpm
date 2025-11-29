#include "process.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cctype>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
#include <sys/resource.h>

// Process constructor
Process::Process() 
    : pid(0), ppid(0), name(""), state(""), user(""),
      vmSize(0), vmRSS(0), utime(0), stime(0), 
      cpuPercent(0.0), numThreads(0), starttime(0) {
}

// Read process information from /proc filesystem
bool Process::readFromProc(int processId) {
    pid = processId;
    
    // Read /proc/<pid>/stat for basic info
    std::string statPath = "/proc/" + std::to_string(pid) + "/stat";
    std::ifstream statFile(statPath);
    if (!statFile.is_open()) {
        return false;
    }
    
    std::string line;
    if (!std::getline(statFile, line)) {
        return false;
    }
    
    // Parse stat file - format: pid (comm) state ppid ...
    size_t start = line.find('(');
    size_t end = line.rfind(')');
    
    if (start == std::string::npos || end == std::string::npos) {
        return false;
    }
    
    name = line.substr(start + 1, end - start - 1);
    
    // Parse remaining fields after comm
    std::istringstream iss(line.substr(end + 2));
    iss >> state >> ppid;
    
    // Skip fields: pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt, cmajflt
    for (int i = 0; i < 9; i++) {
        unsigned long dummy;
        iss >> dummy;
    }
    
    iss >> utime >> stime;
    
    // Skip cutime, cstime, priority, nice
    for (int i = 0; i < 4; i++) {
        long dummy;
        iss >> dummy;
    }
    
    iss >> numThreads;
    
    // Skip itrealvalue
    long dummy;
    iss >> dummy;
    
    iss >> starttime;
    
    // Read /proc/<pid>/status for additional info
    std::string statusPath = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream statusFile(statusPath);
    if (statusFile.is_open()) {
        std::string statusLine;
        int uid = -1;
        
        while (std::getline(statusFile, statusLine)) {
            if (statusLine.find("VmSize:") == 0) {
                std::istringstream vss(statusLine.substr(7));
                vss >> vmSize;
            } else if (statusLine.find("VmRSS:") == 0) {
                std::istringstream vss(statusLine.substr(6));
                vss >> vmRSS;
            } else if (statusLine.find("Uid:") == 0) {
                std::istringstream uss(statusLine.substr(4));
                uss >> uid;
            }
        }
        
        // Get username from UID
        if (uid >= 0) {
            struct passwd* pw = getpwuid(uid);
            if (pw) {
                user = pw->pw_name;
            } else {
                user = std::to_string(uid);
            }
        }
    }
    
    return true;
}

// Calculate CPU percentage
void Process::calculateCPU(unsigned long prevUtime, unsigned long prevStime, 
                          unsigned long totalTimeDiff) {
    if (totalTimeDiff == 0) {
        cpuPercent = 0.0;
        return;
    }
    
    unsigned long processTimeDiff = (utime + stime) - (prevUtime + prevStime);
    cpuPercent = (100.0 * processTimeDiff) / totalTimeDiff;
}

// ProcessManager constructor
ProcessManager::ProcessManager() 
    : lastTotalCPUTime(0), lastUpdateTime(0.0) {
}

// Get all running processes
std::vector<Process> ProcessManager::getAllProcesses() {
    std::vector<Process> processes;
    const std::string procPath = "/proc";
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(procPath)) {
            if (!entry.is_directory()) {
                continue;
            }
            
            std::string dirName = entry.path().filename().string();
            
            // Check if directory name is numeric (PID)
            bool isNumeric = true;
            for (char c : dirName) {
                if (!std::isdigit(c)) {
                    isNumeric = false;
                    break;
                }
            }
            
            if (!isNumeric) {
                continue;
            }
            
            int pid = std::stoi(dirName);
            Process proc;
            
            if (proc.readFromProc(pid)) {
                processes.push_back(proc);
            }
        }
    } catch (const std::exception& e) {
        // Handle filesystem errors silently
    }
    
    return processes;
}

// Get single process by PID
Process ProcessManager::getProcess(int pid) {
    Process proc;
    proc.readFromProc(pid);
    return proc;
}

// Kill a process with specified signal
bool ProcessManager::killProcess(int pid, int signal) {
    if (pid <= 0) {
        return false;
    }
    
    return (kill(pid, signal) == 0);
}

// Change process priority (nice value: -20 to 19)
bool ProcessManager::setProcessPriority(int pid, int priority) {
    if (pid <= 0) {
        return false;
    }
    
    return (setpriority(PRIO_PROCESS, pid, priority) == 0);
}

// Get system uptime in seconds
double ProcessManager::getSystemUptime() {
    std::ifstream uptimeFile("/proc/uptime");
    if (!uptimeFile.is_open()) {
        return 0.0;
    }
    
    double uptime;
    uptimeFile >> uptime;
    return uptime;
}

// Get total CPU time since boot (sum of all CPU time values)
unsigned long ProcessManager::getTotalCPUTime() {
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) {
        return 0;
    }
    
    std::string line;
    std::getline(statFile, line);
    
    // First line is "cpu  user nice system idle iowait irq softirq ..."
    std::istringstream iss(line);
    std::string cpu;
    iss >> cpu; // Skip "cpu"
    
    unsigned long user, nice, system, idle, iowait, irq, softirq;
    iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
    
    return user + nice + system + idle + iowait + irq + softirq;
}
