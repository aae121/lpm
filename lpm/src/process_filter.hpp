#pragma once

#include "process.hpp"
#include <vector>
#include <string>
#include <functional>

// Process filter criteria
class ProcessFilter {
public:
    // Filter by name (substring match, case-insensitive)
    static std::vector<Process> filterByName(const std::vector<Process>& processes,
                                             const std::string& namePattern);
    
    // Filter by user
    static std::vector<Process> filterByUser(const std::vector<Process>& processes,
                                            const std::string& username);
    
    // Filter by state (R, S, D, Z, T, etc.)
    static std::vector<Process> filterByState(const std::vector<Process>& processes,
                                              const std::string& state);
    
    // Filter by PID range
    static std::vector<Process> filterByPIDRange(const std::vector<Process>& processes,
                                                 int minPID, int maxPID);
    
    // Filter by memory usage (min/max in kB)
    static std::vector<Process> filterByMemory(const std::vector<Process>& processes,
                                               long minMemory, long maxMemory);
    
    // Filter by CPU usage percentage
    static std::vector<Process> filterByCPU(const std::vector<Process>& processes,
                                           double minCPU, double maxCPU);
    
    // Filter by parent PID
    static std::vector<Process> filterByParent(const std::vector<Process>& processes,
                                               int ppid);
    
    // Get children of a process
    static std::vector<Process> getChildren(const std::vector<Process>& processes,
                                           int ppid);
    
    // Custom filter with predicate function
    static std::vector<Process> filterCustom(const std::vector<Process>& processes,
                                            std::function<bool(const Process&)> predicate);
    
    // Search for processes (combines name and user search)
    static std::vector<Process> search(const std::vector<Process>& processes,
                                      const std::string& query);

private:
    static std::string toLower(const std::string& str);
};

// Process sorting utilities
class ProcessSorter {
public:
    enum SortField {
        BY_PID,
        BY_NAME,
        BY_USER,
        BY_CPU,
        BY_MEMORY,
        BY_STATE,
        BY_PPID
    };
    
    // Sort processes by specified field
    static void sort(std::vector<Process>& processes, SortField field, 
                    bool ascending = true);
    
    // Get top N processes by CPU
    static std::vector<Process> topByCPU(const std::vector<Process>& processes, 
                                        size_t count);
    
    // Get top N processes by memory
    static std::vector<Process> topByMemory(const std::vector<Process>& processes,
                                           size_t count);

private:
    static bool comparePID(const Process& a, const Process& b, bool ascending);
    static bool compareName(const Process& a, const Process& b, bool ascending);
    static bool compareUser(const Process& a, const Process& b, bool ascending);
    static bool compareCPU(const Process& a, const Process& b, bool ascending);
    static bool compareMemory(const Process& a, const Process& b, bool ascending);
    static bool compareState(const Process& a, const Process& b, bool ascending);
    static bool comparePPID(const Process& a, const Process& b, bool ascending);
};
