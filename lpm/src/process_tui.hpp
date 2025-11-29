#pragma once

#include "process.hpp"
#include "process_filter.hpp"
#include <ncurses.h>
#include <vector>
#include <string>

// Interactive TUI for process management
class ProcessTUI {
public:
    ProcessTUI();
    ~ProcessTUI();
    
    // Initialize ncurses
    bool init();
    
    // Cleanup ncurses
    void cleanup();
    
    // Main event loop
    void run();

private:
    ProcessManager manager;
    std::vector<Process> processes;
    std::vector<Process> filteredProcesses;
    
    int selectedIndex;
    int scrollOffset;
    int screenHeight;
    int screenWidth;
    
    ProcessSorter::SortField sortField;
    bool sortAscending;
    
    std::string searchQuery;
    std::string statusMessage;
    bool showTree;
    double refreshInterval;
    time_t lastRefresh;
    
    // UI Drawing
    void draw();
    void drawHeader();
    void drawProcessList();
    void drawStatusBar();
    void drawHelpBar();
    
    // Event handling
    void handleInput(int ch);
    void handleNavigation(int ch);
    void handleActions(int ch);
    void handleSorting(int ch);
    
    // Actions
    void refreshProcesses();
    void killSelectedProcess();
    void changeSelectedPriority();
    void showProcessDetails();
    void promptSearch();
    void toggleTreeView();
    void showHelp();
    
    // Utilities
    void updateFilter();
    void setStatus(const std::string& message);
    std::string getProcessLine(const Process& proc, int width);
    std::string getStateDescription(const std::string& state);
    void promptInput(const std::string& prompt, std::string& output);
};
