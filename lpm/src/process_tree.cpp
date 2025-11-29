#include "process_tree.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

ProcessTree::ProcessTree() {
}

ProcessTree::~ProcessTree() {
    clear();
}

// Clear the tree and free memory
void ProcessTree::clear() {
    for (auto* root : roots) {
        deleteNode(root);
    }
    roots.clear();
    nodeMap.clear();
}

// Recursively delete nodes
void ProcessTree::deleteNode(ProcessTreeNode* node) {
    if (!node) return;
    
    for (auto* child : node->children) {
        deleteNode(child);
    }
    delete node;
}

// Build process tree from process list
void ProcessTree::buildTree(const std::vector<Process>& processes) {
    clear();
    
    // First pass: create all nodes
    for (const auto& proc : processes) {
        ProcessTreeNode* node = new ProcessTreeNode();
        node->process = proc;
        nodeMap[proc.pid] = node;
    }
    
    // Second pass: build parent-child relationships
    for (auto& pair : nodeMap) {
        ProcessTreeNode* node = pair.second;
        int ppid = node->process.ppid;
        
        // Find parent
        auto parentIt = nodeMap.find(ppid);
        if (parentIt != nodeMap.end()) {
            // Has parent in tree
            parentIt->second->children.push_back(node);
        } else {
            // No parent found - this is a root
            roots.push_back(node);
        }
    }
    
    // Set depth for all nodes
    for (auto* root : roots) {
        buildTreeRecursive(root, 0);
    }
    
    // Sort children by PID for consistent display
    for (auto& pair : nodeMap) {
        std::sort(pair.second->children.begin(), 
                 pair.second->children.end(),
                 [](ProcessTreeNode* a, ProcessTreeNode* b) {
                     return a->process.pid < b->process.pid;
                 });
    }
}

// Set depth recursively
void ProcessTree::buildTreeRecursive(ProcessTreeNode* node, int depth) {
    if (!node) return;
    
    node->depth = depth;
    for (auto* child : node->children) {
        buildTreeRecursive(child, depth + 1);
    }
}

// Print the entire tree
void ProcessTree::printTree(bool showThreads) {
    for (size_t i = 0; i < roots.size(); i++) {
        printNode(roots[i], "", i == roots.size() - 1, showThreads);
    }
}

// Print a single node and its children recursively
void ProcessTree::printNode(ProcessTreeNode* node, const std::string& prefix, 
                           bool isLast, bool showThreads) {
    if (!node) return;
    
    // Print current node
    std::cout << prefix;
    std::cout << (isLast ? "└── " : "├── ");
    
    std::cout << node->process.pid << " " << node->process.name;
    
    if (showThreads && node->process.numThreads > 1) {
        std::cout << " [" << node->process.numThreads << " threads]";
    }
    
    std::cout << " (" << node->process.state << ")";
    
    if (node->process.vmRSS > 0) {
        std::cout << " " << node->process.vmRSS << "kB";
    }
    
    std::cout << "\n";
    
    // Print children
    std::string childPrefix = prefix + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < node->children.size(); i++) {
        printNode(node->children[i], childPrefix, 
                 i == node->children.size() - 1, showThreads);
    }
}

// Get tree as formatted string lines
std::vector<std::string> ProcessTree::getTreeLines(bool showThreads) {
    std::vector<std::string> lines;
    
    for (size_t i = 0; i < roots.size(); i++) {
        getNodeLines(roots[i], "", i == roots.size() - 1, showThreads, lines);
    }
    
    return lines;
}

// Get node lines recursively
void ProcessTree::getNodeLines(ProcessTreeNode* node, const std::string& prefix,
                              bool isLast, bool showThreads,
                              std::vector<std::string>& lines) {
    if (!node) return;
    
    std::ostringstream oss;
    oss << prefix;
    oss << (isLast ? "└── " : "├── ");
    oss << node->process.pid << " " << node->process.name;
    
    if (showThreads && node->process.numThreads > 1) {
        oss << " [" << node->process.numThreads << " threads]";
    }
    
    oss << " (" << node->process.state << ")";
    
    if (node->process.vmRSS > 0) {
        oss << " " << node->process.vmRSS << "kB";
    }
    
    lines.push_back(oss.str());
    
    std::string childPrefix = prefix + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < node->children.size(); i++) {
        getNodeLines(node->children[i], childPrefix,
                    i == node->children.size() - 1, showThreads, lines);
    }
}

// Find process node by PID
ProcessTreeNode* ProcessTree::findProcess(int pid) {
    auto it = nodeMap.find(pid);
    if (it != nodeMap.end()) {
        return it->second;
    }
    return nullptr;
}

// Get all descendant PIDs of a process
std::vector<int> ProcessTree::getDescendants(int pid) {
    std::vector<int> descendants;
    ProcessTreeNode* node = findProcess(pid);
    
    if (node) {
        getDescendantsRecursive(node, descendants);
    }
    
    return descendants;
}

// Recursively collect descendants
void ProcessTree::getDescendantsRecursive(ProcessTreeNode* node, 
                                         std::vector<int>& descendants) {
    if (!node) return;
    
    for (auto* child : node->children) {
        descendants.push_back(child->process.pid);
        getDescendantsRecursive(child, descendants);
    }
}
