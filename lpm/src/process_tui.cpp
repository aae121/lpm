#include "process_tui.hpp"
#include "process_control.hpp"
#include "process_tree.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>
#include <unistd.h>

ProcessTUI::ProcessTUI() 
    : selectedIndex(0), scrollOffset(0), screenHeight(0), screenWidth(0),
      sortField(ProcessSorter::BY_PID), sortAscending(true),
      searchQuery(""), statusMessage(""), showTree(false),
      refreshInterval(2.0), lastRefresh(0) {
}

ProcessTUI::~ProcessTUI() {
    cleanup();
}

// Initialize ncurses
bool ProcessTUI::init() {
    initscr();              // Initialize screen
    cbreak();               // Disable line buffering
    noecho();               // Don't echo keypresses
    keypad(stdscr, TRUE);   // Enable function keys
    nodelay(stdscr, TRUE);  // Non-blocking getch()
    curs_set(0);            // Hide cursor
    
    // Check if colors are supported
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);    // Header
        init_pair(2, COLOR_BLACK, COLOR_WHITE);   // Selected
        init_pair(3, COLOR_GREEN, COLOR_BLACK);   // Normal
        init_pair(4, COLOR_RED, COLOR_BLACK);     // Error
        init_pair(5, COLOR_YELLOW, COLOR_BLACK);  // Warning
    }
    
    getmaxyx(stdscr, screenHeight, screenWidth);
    
    refreshProcesses();
    setStatus("LPM Started - Press 'h' for help");
    
    return true;
}

// Cleanup ncurses
void ProcessTUI::cleanup() {
    if (isendwin() == FALSE) {
        endwin();
    }
}

// Main event loop
void ProcessTUI::run() {
    bool running = true;
    
    while (running) {
        // Check if terminal was resized
        getmaxyx(stdscr, screenHeight, screenWidth);
        
        // Auto-refresh
        time_t now = time(nullptr);
        if (now - lastRefresh >= refreshInterval) {
            refreshProcesses();
            lastRefresh = now;
        }
        
        draw();
        
        int ch = getch();
        if (ch != ERR) {
            if (ch == 'q' || ch == 'Q') {
                running = false;
            } else {
                handleInput(ch);
            }
        }
        
        usleep(50000); // 50ms delay
    }
}

// Draw the entire UI
void ProcessTUI::draw() {
    clear();
    
    drawHeader();
    drawProcessList();
    drawStatusBar();
    drawHelpBar();
    
    refresh();
}

// Draw header with system info
void ProcessTUI::drawHeader() {
    attron(COLOR_PAIR(1) | A_BOLD);
    
    std::string title = " Linux Process Manager (LPM) ";
    int titlePos = (screenWidth - title.length()) / 2;
    
    // Fill entire line with background color
    for (int i = 0; i < screenWidth; i++) {
        mvaddch(0, i, ' ');
    }
    
    mvprintw(0, titlePos, "%s", title.c_str());
    
    // Show process count
    std::string count = "Processes: " + std::to_string(filteredProcesses.size()) + 
                       "/" + std::to_string(processes.size()) + " ";
    mvprintw(0, screenWidth - count.length(), "%s", count.c_str());
    
    attroff(COLOR_PAIR(1) | A_BOLD);
}

// Draw process list
void ProcessTUI::drawProcessList() {
    int startY = 2;
    int listHeight = screenHeight - 5;
    
    // Draw column headers
    attron(A_BOLD);
    mvprintw(startY, 0, "%-8s %-20s %-10s %-8s %-10s %s", 
             "PID", "NAME", "USER", "STATE", "MEMORY", "CPU%");
    attroff(A_BOLD);
    
    // Draw processes
    int displayCount = std::min((int)filteredProcesses.size() - scrollOffset, listHeight);
    
    for (int i = 0; i < displayCount; i++) {
        int procIndex = scrollOffset + i;
        const Process& proc = filteredProcesses[procIndex];
        
        int y = startY + 1 + i;
        
        // Highlight selected
        if (procIndex == selectedIndex) {
            attron(COLOR_PAIR(2) | A_BOLD);
        }
        
        std::string memStr = proc.vmRSS > 0 ? 
                            std::to_string(proc.vmRSS / 1024) + "M" : "N/A";
        
        mvprintw(y, 0, "%-8d %-20.20s %-10.10s %-8s %-10s %.2f", 
                 proc.pid, proc.name.c_str(), proc.user.c_str(),
                 proc.state.c_str(), memStr.c_str(), proc.cpuPercent);
        
        if (procIndex == selectedIndex) {
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
    }
}

// Draw status bar
void ProcessTUI::drawStatusBar() {
    int y = screenHeight - 2;
    
    attron(COLOR_PAIR(3));
    mvprintw(y, 0, "%s", statusMessage.c_str());
    attroff(COLOR_PAIR(3));
}

// Draw help bar
void ProcessTUI::drawHelpBar() {
    int y = screenHeight - 1;
    
    std::string help = "[q]Quit [r]Refresh [k]Kill [n]Nice [/]Search [s]Sort [t]Tree [h]Help";
    
    attron(COLOR_PAIR(1));
    for (int i = 0; i < screenWidth; i++) {
        mvaddch(y, i, ' ');
    }
    mvprintw(y, 0, "%s", help.c_str());
    attroff(COLOR_PAIR(1));
}

// Handle keyboard input
void ProcessTUI::handleInput(int ch) {
    handleNavigation(ch);
    handleActions(ch);
    handleSorting(ch);
}

// Handle navigation keys
void ProcessTUI::handleNavigation(int ch) {
    int listHeight = screenHeight - 5;
    
    switch (ch) {
        case KEY_UP:
        case 'k':
            if (selectedIndex > 0) {
                selectedIndex--;
                if (selectedIndex < scrollOffset) {
                    scrollOffset = selectedIndex;
                }
            }
            break;
            
        case KEY_DOWN:
        case 'j':
            if (selectedIndex < (int)filteredProcesses.size() - 1) {
                selectedIndex++;
                if (selectedIndex >= scrollOffset + listHeight) {
                    scrollOffset = selectedIndex - listHeight + 1;
                }
            }
            break;
            
        case KEY_PPAGE: // Page Up
            selectedIndex = std::max(0, selectedIndex - listHeight);
            scrollOffset = std::max(0, scrollOffset - listHeight);
            break;
            
        case KEY_NPAGE: // Page Down
            selectedIndex = std::min((int)filteredProcesses.size() - 1, 
                                   selectedIndex + listHeight);
            scrollOffset = std::min((int)filteredProcesses.size() - listHeight,
                                  scrollOffset + listHeight);
            break;
    }
}

// Handle action keys
void ProcessTUI::handleActions(int ch) {
    switch (ch) {
        case 'r':
        case 'R':
            refreshProcesses();
            setStatus("Processes refreshed");
            break;
            
        case 'k':
        case 'K':
            killSelectedProcess();
            break;
            
        case 'n':
        case 'N':
            changeSelectedPriority();
            break;
            
        case '/':
            promptSearch();
            break;
            
        case 't':
        case 'T':
            toggleTreeView();
            break;
            
        case 'h':
        case 'H':
        case KEY_F(1):
            showHelp();
            break;
            
        case '\n':
        case KEY_ENTER:
            showProcessDetails();
            break;
    }
}

// Handle sorting keys
void ProcessTUI::handleSorting(int ch) {
    bool needSort = false;
    
    switch (ch) {
        case 'P':
            sortField = ProcessSorter::BY_PID;
            needSort = true;
            break;
        case 'N':
            sortField = ProcessSorter::BY_NAME;
            needSort = true;
            break;
        case 'M':
            sortField = ProcessSorter::BY_MEMORY;
            needSort = true;
            break;
        case 'C':
            sortField = ProcessSorter::BY_CPU;
            needSort = true;
            break;
        case 'U':
            sortField = ProcessSorter::BY_USER;
            needSort = true;
            break;
    }
    
    if (needSort) {
        sortAscending = !sortAscending;
        ProcessSorter::sort(filteredProcesses, sortField, sortAscending);
        setStatus("Sorted by field");
    }
}

// Refresh process list
void ProcessTUI::refreshProcesses() {
    processes = manager.getAllProcesses();
    updateFilter();
    
    // Adjust selection if needed
    if (selectedIndex >= (int)filteredProcesses.size()) {
        selectedIndex = std::max(0, (int)filteredProcesses.size() - 1);
    }
}

// Kill selected process
void ProcessTUI::killSelectedProcess() {
    if (selectedIndex >= 0 && selectedIndex < (int)filteredProcesses.size()) {
        int pid = filteredProcesses[selectedIndex].pid;
        
        // Confirm
        setStatus("Kill process " + std::to_string(pid) + "? (y/n)");
        draw();
        
        nodelay(stdscr, FALSE);
        int ch = getch();
        nodelay(stdscr, TRUE);
        
        if (ch == 'y' || ch == 'Y') {
            if (ProcessController::terminate(pid)) {
                setStatus("Process " + std::to_string(pid) + " terminated");
                refreshProcesses();
            } else {
                setStatus("Failed to kill process " + std::to_string(pid));
            }
        } else {
            setStatus("Cancelled");
        }
    }
}

// Change process priority
void ProcessTUI::changeSelectedPriority() {
    if (selectedIndex >= 0 && selectedIndex < (int)filteredProcesses.size()) {
        int pid = filteredProcesses[selectedIndex].pid;
        setStatus("Enter new nice value (-20 to 19): ");
        // Implementation would require input handling
        setStatus("Priority change not implemented in this demo");
    }
}

// Show process details
void ProcessTUI::showProcessDetails() {
    // Would show detailed view of selected process
    setStatus("Details view not implemented");
}

// Prompt for search
void ProcessTUI::promptSearch() {
    setStatus("Search not fully implemented");
}

// Toggle tree view
void ProcessTUI::toggleTreeView() {
    showTree = !showTree;
    setStatus(showTree ? "Tree view enabled" : "List view enabled");
}

// Show help dialog
void ProcessTUI::showHelp() {
    clear();
    int y = 2;
    
    mvprintw(y++, 2, "=== LPM Help ===");
    y++;
    mvprintw(y++, 2, "Navigation:");
    mvprintw(y++, 4, "Up/Down, j/k  - Move selection");
    mvprintw(y++, 4, "PgUp/PgDn     - Page up/down");
    y++;
    mvprintw(y++, 2, "Actions:");
    mvprintw(y++, 4, "r - Refresh");
    mvprintw(y++, 4, "k - Kill process");
    mvprintw(y++, 4, "n - Change priority (nice)");
    mvprintw(y++, 4, "/ - Search");
    mvprintw(y++, 4, "t - Toggle tree view");
    y++;
    mvprintw(y++, 2, "Sorting (toggle ascending/descending):");
    mvprintw(y++, 4, "P - Sort by PID");
    mvprintw(y++, 4, "N - Sort by Name");
    mvprintw(y++, 4, "M - Sort by Memory");
    mvprintw(y++, 4, "C - Sort by CPU");
    mvprintw(y++, 4, "U - Sort by User");
    y++;
    mvprintw(y++, 2, "q - Quit");
    y++;
    mvprintw(y++, 2, "Press any key to continue...");
    
    refresh();
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);
}

// Update filtered process list
void ProcessTUI::updateFilter() {
    if (searchQuery.empty()) {
        filteredProcesses = processes;
    } else {
        filteredProcesses = ProcessFilter::search(processes, searchQuery);
    }
    
    ProcessSorter::sort(filteredProcesses, sortField, sortAscending);
}

// Set status message
void ProcessTUI::setStatus(const std::string& message) {
    statusMessage = message;
}

// Get state description
std::string ProcessTUI::getStateDescription(const std::string& state) {
    if (state == "R") return "Running";
    if (state == "S") return "Sleeping";
    if (state == "D") return "Disk Sleep";
    if (state == "Z") return "Zombie";
    if (state == "T") return "Stopped";
    if (state == "t") return "Tracing";
    if (state == "X") return "Dead";
    return "Unknown";
}
