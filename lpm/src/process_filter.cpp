#include "process_filter.hpp"
#include <algorithm>
#include <cctype>

// Helper: Convert string to lowercase
std::string ProcessFilter::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Filter by name (substring match, case-insensitive)
std::vector<Process> ProcessFilter::filterByName(const std::vector<Process>& processes,
                                                const std::string& namePattern) {
    std::vector<Process> result;
    std::string lowerPattern = toLower(namePattern);
    
    for (const auto& proc : processes) {
        std::string lowerName = toLower(proc.name);
        if (lowerName.find(lowerPattern) != std::string::npos) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// Filter by user
std::vector<Process> ProcessFilter::filterByUser(const std::vector<Process>& processes,
                                                const std::string& username) {
    std::vector<Process> result;
    
    for (const auto& proc : processes) {
        if (proc.user == username) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// Filter by state
std::vector<Process> ProcessFilter::filterByState(const std::vector<Process>& processes,
                                                  const std::string& state) {
    std::vector<Process> result;
    
    for (const auto& proc : processes) {
        if (proc.state == state) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// Filter by PID range
std::vector<Process> ProcessFilter::filterByPIDRange(const std::vector<Process>& processes,
                                                     int minPID, int maxPID) {
    std::vector<Process> result;
    
    for (const auto& proc : processes) {
        if (proc.pid >= minPID && proc.pid <= maxPID) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// Filter by memory usage
std::vector<Process> ProcessFilter::filterByMemory(const std::vector<Process>& processes,
                                                   long minMemory, long maxMemory) {
    std::vector<Process> result;
    
    for (const auto& proc : processes) {
        if (proc.vmRSS >= minMemory && proc.vmRSS <= maxMemory) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// Filter by CPU usage
std::vector<Process> ProcessFilter::filterByCPU(const std::vector<Process>& processes,
                                               double minCPU, double maxCPU) {
    std::vector<Process> result;
    
    for (const auto& proc : processes) {
        if (proc.cpuPercent >= minCPU && proc.cpuPercent <= maxCPU) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// Filter by parent PID
std::vector<Process> ProcessFilter::filterByParent(const std::vector<Process>& processes,
                                                   int ppid) {
    std::vector<Process> result;
    
    for (const auto& proc : processes) {
        if (proc.ppid == ppid) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// Get children of a process
std::vector<Process> ProcessFilter::getChildren(const std::vector<Process>& processes,
                                               int ppid) {
    return filterByParent(processes, ppid);
}

// Custom filter with predicate
std::vector<Process> ProcessFilter::filterCustom(const std::vector<Process>& processes,
                                                std::function<bool(const Process&)> predicate) {
    std::vector<Process> result;
    
    for (const auto& proc : processes) {
        if (predicate(proc)) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// Search for processes (name or user match)
std::vector<Process> ProcessFilter::search(const std::vector<Process>& processes,
                                          const std::string& query) {
    std::vector<Process> result;
    std::string lowerQuery = toLower(query);
    
    for (const auto& proc : processes) {
        std::string lowerName = toLower(proc.name);
        std::string lowerUser = toLower(proc.user);
        
        if (lowerName.find(lowerQuery) != std::string::npos ||
            lowerUser.find(lowerQuery) != std::string::npos) {
            result.push_back(proc);
        }
    }
    
    return result;
}

// ProcessSorter implementations

bool ProcessSorter::comparePID(const Process& a, const Process& b, bool ascending) {
    return ascending ? (a.pid < b.pid) : (a.pid > b.pid);
}

bool ProcessSorter::compareName(const Process& a, const Process& b, bool ascending) {
    return ascending ? (a.name < b.name) : (a.name > b.name);
}

bool ProcessSorter::compareUser(const Process& a, const Process& b, bool ascending) {
    return ascending ? (a.user < b.user) : (a.user > b.user);
}

bool ProcessSorter::compareCPU(const Process& a, const Process& b, bool ascending) {
    return ascending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
}

bool ProcessSorter::compareMemory(const Process& a, const Process& b, bool ascending) {
    return ascending ? (a.vmRSS < b.vmRSS) : (a.vmRSS > b.vmRSS);
}

bool ProcessSorter::compareState(const Process& a, const Process& b, bool ascending) {
    return ascending ? (a.state < b.state) : (a.state > b.state);
}

bool ProcessSorter::comparePPID(const Process& a, const Process& b, bool ascending) {
    return ascending ? (a.ppid < b.ppid) : (a.ppid > b.ppid);
}

// Sort processes by specified field
void ProcessSorter::sort(std::vector<Process>& processes, SortField field, bool ascending) {
    switch (field) {
        case BY_PID:
            std::sort(processes.begin(), processes.end(),
                     [ascending](const Process& a, const Process& b) {
                         return comparePID(a, b, ascending);
                     });
            break;
        case BY_NAME:
            std::sort(processes.begin(), processes.end(),
                     [ascending](const Process& a, const Process& b) {
                         return compareName(a, b, ascending);
                     });
            break;
        case BY_USER:
            std::sort(processes.begin(), processes.end(),
                     [ascending](const Process& a, const Process& b) {
                         return compareUser(a, b, ascending);
                     });
            break;
        case BY_CPU:
            std::sort(processes.begin(), processes.end(),
                     [ascending](const Process& a, const Process& b) {
                         return compareCPU(a, b, ascending);
                     });
            break;
        case BY_MEMORY:
            std::sort(processes.begin(), processes.end(),
                     [ascending](const Process& a, const Process& b) {
                         return compareMemory(a, b, ascending);
                     });
            break;
        case BY_STATE:
            std::sort(processes.begin(), processes.end(),
                     [ascending](const Process& a, const Process& b) {
                         return compareState(a, b, ascending);
                     });
            break;
        case BY_PPID:
            std::sort(processes.begin(), processes.end(),
                     [ascending](const Process& a, const Process& b) {
                         return comparePPID(a, b, ascending);
                     });
            break;
    }
}

// Get top N processes by CPU usage
std::vector<Process> ProcessSorter::topByCPU(const std::vector<Process>& processes, 
                                            size_t count) {
    std::vector<Process> sorted = processes;
    sort(sorted, BY_CPU, false); // Descending
    
    if (sorted.size() > count) {
        sorted.resize(count);
    }
    
    return sorted;
}

// Get top N processes by memory usage
std::vector<Process> ProcessSorter::topByMemory(const std::vector<Process>& processes,
                                               size_t count) {
    std::vector<Process> sorted = processes;
    sort(sorted, BY_MEMORY, false); // Descending
    
    if (sorted.size() > count) {
        sorted.resize(count);
    }
    
    return sorted;
}
