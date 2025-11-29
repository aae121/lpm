#pragma once

#include "process.hpp"
#include <vector>
#include <map>
#include <string>

// Node in the process tree
struct ProcessTreeNode {
    Process process;
    std::vector<ProcessTreeNode*> children;
    int depth;
    
    ProcessTreeNode() : depth(0) {}
};

// Process tree builder and visualizer
class ProcessTree {
public:
    ProcessTree();
    ~ProcessTree();
    
    // Build tree from process list
    void buildTree(const std::vector<Process>& processes);
    
    // Clear the tree
    void clear();
    
    // Print tree to output
    void printTree(bool showThreads = false);
    
    // Get tree as formatted strings
    std::vector<std::string> getTreeLines(bool showThreads = false);
    
    // Find process in tree
    ProcessTreeNode* findProcess(int pid);
    
    // Get all descendants of a process
    std::vector<int> getDescendants(int pid);

private:
    std::map<int, ProcessTreeNode*> nodeMap;
    std::vector<ProcessTreeNode*> roots;
    
    void buildTreeRecursive(ProcessTreeNode* node, int depth);
    void printNode(ProcessTreeNode* node, const std::string& prefix, 
                   bool isLast, bool showThreads);
    void getNodeLines(ProcessTreeNode* node, const std::string& prefix,
                     bool isLast, bool showThreads, 
                     std::vector<std::string>& lines);
    void deleteNode(ProcessTreeNode* node);
    void getDescendantsRecursive(ProcessTreeNode* node, std::vector<int>& descendants);
};
