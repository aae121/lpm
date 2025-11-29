#include "process.hpp"
#include "process_control.hpp"
#include "process_tree.hpp"
#include "process_filter.hpp"
#include "process_tui.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>

// Function prototypes
void printUsage(const char* programName);
void listProcesses(bool detailed);
void showProcessTree();
void searchProcesses(const std::string& query);
void killProcess(int pid, int signal);
void showTopProcesses(int count);
void interactiveMode();

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc == 1) {
        // No arguments - run interactive TUI
        interactiveMode();
        return 0;
    }
    
    std::string command = argv[1];
    
    if (command == "--help" || command == "-h") {
        printUsage(argv[0]);
        return 0;
    }
    else if (command == "--list" || command == "-l") {
        bool detailed = false;
        if (argc > 2 && std::string(argv[2]) == "--detailed") {
            detailed = true;
        }
        listProcesses(detailed);
    }
    else if (command == "--tree" || command == "-t") {
        showProcessTree();
    }
    else if (command == "--search" || command == "-s") {
        if (argc < 3) {
            std::cerr << "Error: --search requires a search query\n";
            return 1;
        }
        searchProcesses(argv[2]);
    }
    else if (command == "--kill" || command == "-k") {
        if (argc < 3) {
            std::cerr << "Error: --kill requires a PID\n";
            return 1;
        }
        int pid = std::atoi(argv[2]);
        int signal = 15; // SIGTERM by default
        if (argc > 3) {
            signal = std::atoi(argv[3]);
        }
        killProcess(pid, signal);
    }
    else if (command == "--top") {
        int count = 10;
        if (argc > 2) {
            count = std::atoi(argv[2]);
        }
        showTopProcesses(count);
    }
    else if (command == "--interactive" || command == "-i") {
        interactiveMode();
    }
    else {
        std::cerr << "Unknown command: " << command << "\n";
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}

void printUsage(const char* programName) {
    std::cout << "Linux Process Manager (LPM)\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << "                         Run interactive TUI mode\n";
    std::cout << "  " << programName << " --list [-l]            List all processes\n";
    std::cout << "  " << programName << " --list --detailed      List with detailed info\n";
    std::cout << "  " << programName << " --tree [-t]            Show process tree\n";
    std::cout << "  " << programName << " --search [-s] QUERY    Search for processes\n";
    std::cout << "  " << programName << " --kill [-k] PID [SIG]  Kill process (default: SIGTERM)\n";
    std::cout << "  " << programName << " --top [N]              Show top N processes by CPU\n";
    std::cout << "  " << programName << " --interactive [-i]     Run interactive mode\n";
    std::cout << "  " << programName << " --help [-h]            Show this help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --list\n";
    std::cout << "  " << programName << " --search firefox\n";
    std::cout << "  " << programName << " --kill 1234\n";
    std::cout << "  " << programName << " --top 20\n";
}

void listProcesses(bool detailed) {
    ProcessManager manager;
    std::vector<Process> processes = manager.getAllProcesses();
    
    ProcessSorter::sort(processes, ProcessSorter::BY_PID, true);
    
    std::cout << "Total processes: " << processes.size() << "\n\n";
    
    if (detailed) {
        std::cout << std::left 
                  << std::setw(8) << "PID"
                  << std::setw(8) << "PPID"
                  << std::setw(20) << "NAME"
                  << std::setw(12) << "USER"
                  << std::setw(8) << "STATE"
                  << std::setw(12) << "MEMORY(MB)"
                  << std::setw(8) << "THREADS"
                  << "\n";
        std::cout << std::string(80, '-') << "\n";
        
        for (const auto& proc : processes) {
            std::cout << std::left
                      << std::setw(8) << proc.pid
                      << std::setw(8) << proc.ppid
                      << std::setw(20) << proc.name.substr(0, 19)
                      << std::setw(12) << proc.user.substr(0, 11)
                      << std::setw(8) << proc.state
                      << std::setw(12) << (proc.vmRSS / 1024)
                      << std::setw(8) << proc.numThreads
                      << "\n";
        }
    } else {
        std::cout << std::left
                  << std::setw(8) << "PID"
                  << std::setw(24) << "NAME"
                  << std::setw(12) << "MEMORY(kB)"
                  << "\n";
        std::cout << std::string(44, '-') << "\n";
        
        for (const auto& proc : processes) {
            std::cout << std::left
                      << std::setw(8) << proc.pid
                      << std::setw(24) << proc.name.substr(0, 23)
                      << std::setw(12) << proc.vmRSS
                      << "\n";
        }
    }
}

void showProcessTree() {
    ProcessManager manager;
    std::vector<Process> processes = manager.getAllProcesses();
    
    std::cout << "Process Tree (Total: " << processes.size() << " processes)\n\n";
    
    ProcessTree tree;
    tree.buildTree(processes);
    tree.printTree(true);
}

void searchProcesses(const std::string& query) {
    ProcessManager manager;
    std::vector<Process> processes = manager.getAllProcesses();
    std::vector<Process> results = ProcessFilter::search(processes, query);
    
    std::cout << "Search results for: \"" << query << "\"\n";
    std::cout << "Found " << results.size() << " matching processes\n\n";
    
    std::cout << std::left
              << std::setw(8) << "PID"
              << std::setw(24) << "NAME"
              << std::setw(12) << "USER"
              << std::setw(12) << "MEMORY(MB)"
              << "\n";
    std::cout << std::string(56, '-') << "\n";
    
    for (const auto& proc : results) {
        std::cout << std::left
                  << std::setw(8) << proc.pid
                  << std::setw(24) << proc.name.substr(0, 23)
                  << std::setw(12) << proc.user.substr(0, 11)
                  << std::setw(12) << (proc.vmRSS / 1024)
                  << "\n";
    }
}

void killProcess(int pid, int signal) {
    std::cout << "Attempting to send signal " << signal << " to process " << pid << "...\n";
    
    if (ProcessController::sendSignal(pid, signal)) {
        std::cout << "Signal sent successfully.\n";
    } else {
        std::cerr << "Failed to send signal. Error: " << strerror(errno) << "\n";
        std::cerr << "You may need root privileges.\n";
    }
}

void showTopProcesses(int count) {
    ProcessManager manager;
    std::vector<Process> processes = manager.getAllProcesses();
    
    std::cout << "Top " << count << " processes by memory usage:\n\n";
    
    std::vector<Process> topProcs = ProcessSorter::topByMemory(processes, count);
    
    std::cout << std::left
              << std::setw(8) << "PID"
              << std::setw(24) << "NAME"
              << std::setw(12) << "USER"
              << std::setw(12) << "MEMORY(MB)"
              << std::setw(8) << "THREADS"
              << "\n";
    std::cout << std::string(64, '-') << "\n";
    
    for (const auto& proc : topProcs) {
        std::cout << std::left
                  << std::setw(8) << proc.pid
                  << std::setw(24) << proc.name.substr(0, 23)
                  << std::setw(12) << proc.user.substr(0, 11)
                  << std::setw(12) << (proc.vmRSS / 1024)
                  << std::setw(8) << proc.numThreads
                  << "\n";
    }
}

void interactiveMode() {
    ProcessTUI tui;
    
    if (!tui.init()) {
        std::cerr << "Failed to initialize TUI\n";
        return;
    }
    
    tui.run();
}
