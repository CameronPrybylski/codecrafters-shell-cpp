#pragma once

#include <string>
#include <set>
#include <vector>

class Shell {
public:
    Shell();

    std::vector<std::string> ParseInput();
    bool HandleCommands(std::vector<std::string> commands);
    void Echo(std::string input);
    void Type(std::string input);
    void Path(std::string input);
    void PathTo(std::string input);
    void CommandNotFound(std::string input);
    
    void Run();
private:
    std::set<std::string> builtins;
    std::set<std::string> paths;
};