# Linux Process Manager (LPM)

## Purpose

LPM is a comprehensive command-line process manager for Linux systems. It provides advanced functionality to monitor, analyze, and manage running processes by interacting directly with the Linux `/proc` filesystem. The tool offers both interactive TUI and command-line interfaces for maximum flexibility.

## Features

### Core Functionality

- **Process Listing**: Enumerate all running processes with detailed information
  - PID, PPID, process name, user, state
  - Memory usage (VmSize, VmRSS)
  - CPU usage percentage
  - Thread count

- **Interactive TUI**: Full-featured ncurses-based interface
  - Real-time process monitoring with auto-refresh
  - Keyboard navigation (vim-style and arrow keys)
  - Multiple sort options (PID, name, CPU, memory, user)
  - Process search and filtering
  - Color-coded display with selection highlighting

- **Process Control**: Manage processes with various operations
  - Send signals (SIGTERM, SIGKILL, SIGSTOP, SIGCONT, etc.)
  - Change process priority (nice/renice)
  - Kill processes safely with confirmation

- **Process Tree**: Hierarchical visualization of process relationships
  - Parent-child relationship display
  - Recursive tree structure with proper indentation
  - Thread count display
  - Process state indicators

- **Filtering & Search**: Powerful search capabilities
  - Filter by name (substring match, case-insensitive)
  - Filter by user, state, PID range
  - Filter by memory usage or CPU percentage
  - Search by parent PID

- **Sorting**: Multiple sorting options
  - Sort by PID, name, user, state, PPID
  - Sort by CPU usage or memory consumption
  - Toggle ascending/descending order

- **Top Processes**: Quick view of resource-intensive processes
  - Top N processes by CPU usage
  - Top N processes by memory usage

## Build Instructions

### Prerequisites

- Linux operating system
- GCC with C++17 support (g++ 7.0 or higher)
- GNU Make
- ncurses library (`libncurses-dev` on Debian/Ubuntu)

### Install Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get install build-essential libncurses5-dev libncurses-dev
```

On Fedora/RHEL:
```bash
sudo dnf install gcc-c++ ncurses-devel
```

On Arch Linux:
```bash
sudo pacman -S gcc ncurses
```

### Compile

To compile the project, run:

```bash
make
```

This will:
- Create a `build/` directory for object files
- Compile all source files with optimization enabled
- Link with ncurses library
- Generate the `lpm` executable

## Run Instructions

### Interactive TUI Mode (Default)

Run without arguments for the interactive interface:

```bash
./lpm
```

or

```bash
make run
```

#### TUI Keyboard Controls

**Navigation:**
- `↑/↓` or `j/k` - Move selection up/down
- `PgUp/PgDn` - Page up/down

**Actions:**
- `r` - Refresh process list
- `k` - Kill selected process (with confirmation)
- `n` - Change process priority (nice value)
- `/` - Search for processes
- `t` - Toggle tree view
- `h` - Show help screen
- `q` - Quit

**Sorting:**
- `P` - Sort by PID
- `N` - Sort by Name
- `M` - Sort by Memory
- `C` - Sort by CPU
- `U` - Sort by User

### Command-Line Mode

#### List All Processes

```bash
./lpm --list
```

With detailed information:

```bash
./lpm --list --detailed
```

or

```bash
make list
```

#### Show Process Tree

```bash
./lpm --tree
```

or

```bash
make tree
```

#### Search for Processes

```bash
./lpm --search firefox
./lpm --search chrome
```

#### Kill a Process

Send SIGTERM (graceful termination):
```bash
./lpm --kill 1234
```

Send specific signal:
```bash
./lpm --kill 1234 9    # SIGKILL (force)
./lpm --kill 1234 15   # SIGTERM (default)
./lpm --kill 1234 19   # SIGSTOP (suspend)
```

#### Show Top Processes

Top 10 by memory (default):
```bash
./lpm --top
```

Top 20 by memory:
```bash
./lpm --top 20
```

or

```bash
make top
```

#### Get Help

```bash
./lpm --help
```

### Installation (Optional)

Install system-wide (requires root):

```bash
sudo make install
```

This installs to `/usr/local/bin/lpm`. Uninstall with:

```bash
sudo make uninstall
```

## Clean Build Artifacts

To clean build artifacts:

```bash
make clean
```

## Architecture

### Module Structure

```
lpm/
├── src/
│   ├── main.cpp              - Entry point and CLI argument parsing
│   ├── process.hpp/cpp       - Process data structure and manager
│   ├── process_control.hpp/cpp    - Process control operations (signals, priority)
│   ├── process_tree.hpp/cpp       - Process tree builder and visualizer
│   ├── process_filter.hpp/cpp     - Filtering and sorting utilities
│   ├── process_tui.hpp/cpp        - Interactive TUI with ncurses
│   └── process_list.hpp/cpp       - Legacy simple listing (deprecated)
├── build/                    - Compiled object files
├── Makefile                  - Build system
└── README.md                - Documentation
```

### Design Principles

- **Modular Architecture**: Each component is separated into focused modules
- **Clean C++17**: Modern C++ features, no raw pointers, RAII principles
- **Error Handling**: Graceful handling of permission errors and missing files
- **No Global Variables**: All state encapsulated in classes
- **Performance**: Efficient /proc parsing with minimal overhead

## Usage Examples

### Example 1: Monitor System Resources

```bash
# Interactive mode with real-time updates
./lpm

# Sort by memory usage and see top consumers
# Press 'M' in interactive mode
```

### Example 2: Find and Kill Processes

```bash
# Search for Chrome processes
./lpm --search chrome

# Kill a specific Chrome process
./lpm --kill 5678

# Force kill if necessary
./lpm --kill 5678 9
```

### Example 3: Analyze Process Hierarchy

```bash
# View complete process tree
./lpm --tree

# See parent-child relationships
```

### Example 4: Monitor Specific User

```bash
# List all processes (detailed)
./lpm --list --detailed

# Then filter in your shell
./lpm --list --detailed | grep username
```

## Future Enhancements

Planned features for future releases:

- **System Monitoring Dashboard**
  - Overall CPU usage graph
  - Memory usage statistics
  - Load average display
  - Disk I/O monitoring

- **Advanced Features**
  - cgroups integration and control
  - systemd service management
  - Container/Docker process support
  - Network connection tracking per process

- **Persistence & History**
  - Process history logging
  - Resource usage trends
  - Alert configuration
  - Custom process groups

- **Multi-Host Support**
  - Remote process monitoring via SSH
  - Distributed process management
  - Cluster-wide views

- **Enhanced UI**
  - Graph-based tree visualization
  - Configurable color schemes
  - Mouse support in TUI
  - Optional GUI with Qt/GTK

## Requirements

- Linux operating system with /proc filesystem
- GCC with C++17 support (g++ 7.0 or higher)
- GNU Make
- ncurses library (version 5.0 or higher)
- Standard C++ library with filesystem support

## Testing

To test the application:

1. **Basic functionality test:**
   ```bash
   make
   ./lpm --list
   ```

2. **Interactive mode test:**
   ```bash
   ./lpm
   # Navigate with arrow keys
   # Try sorting with P, N, M, C keys
   # Press 'q' to quit
   ```

3. **Process control test (be careful!):**
   ```bash
   # Start a sleep process
   sleep 1000 &
   
   # Find its PID
   ./lpm --search sleep
   
   # Kill it (replace XXXX with actual PID)
   ./lpm --kill XXXX
   ```

4. **Tree view test:**
   ```bash
   ./lpm --tree | head -50
   ```

## Troubleshooting

### Permission Denied Errors

Some operations require root privileges:
```bash
sudo ./lpm --kill 1234
```

### Missing ncurses Library

Install development headers:
```bash
# Ubuntu/Debian
sudo apt-get install libncurses-dev

# Fedora
sudo dnf install ncurses-devel
```

### TUI Display Issues

Ensure your terminal supports ncurses:
```bash
echo $TERM
# Should show something like xterm-256color
```

## License

This is an educational project developed as part of a term project assignment.

## Contributors

See project cover page for full list of contributors and their roles.

## References

- Linux `/proc` filesystem documentation: https://www.kernel.org/doc/Documentation/filesystems/proc.txt
- ncurses programming guide: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
- Process management on Linux: https://www.makeuseof.com/linux-process-management/
