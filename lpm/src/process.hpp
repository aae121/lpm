#pragma once

#include <string>
#include <vector>

// Represents a single process with all its attributes
class Process {
public:
    int pid;
    int ppid;              // Parent process ID
    std::string name;
    std::string state;     // R, S, D, Z, T, etc.
    std::string user;
    long vmSize;           // Virtual memory size in kB
    long vmRSS;            // Resident set size in kB
    unsigned long utime;   // CPU time in user mode (jiffies)
    unsigned long stime;   // CPU time in kernel mode (jiffies)
    double cpuPercent;     // CPU usage percentage
    int numThreads;
    long starttime;        // Process start time (jiffies since boot)

    Process();
    
    // Read process information from /proc/<pid>
    bool readFromProc(int processId);
    
    // Calculate CPU percentage based on previous sample
    void calculateCPU(unsigned long prevUtime, unsigned long prevStime, 
                     unsigned long totalTimeDiff);
};

// Process information collector
class ProcessManager {
public:
    ProcessManager();
    
    // Get all running processes
    std::vector<Process> getAllProcesses();
    
    // Get process by PID
    Process getProcess(int pid);
    
    // Kill a process with specified signal
    bool killProcess(int pid, int signal = 15); // SIGTERM by default
    
    // Change process priority
    bool setProcessPriority(int pid, int priority);
    
    // Get system uptime in seconds
    double getSystemUptime();
    
    // Get total CPU time since boot (for CPU % calculations)
    unsigned long getTotalCPUTime();

private:
    unsigned long lastTotalCPUTime;
    double lastUpdateTime;
    
    // Helper to read /proc/<pid>/stat
    bool readStat(int pid, Process& proc);
    
    // Helper to read /proc/<pid>/status
    bool readStatus(int pid, Process& proc);
    
    // Helper to get username from UID
    std::string getUsername(int uid);
};
