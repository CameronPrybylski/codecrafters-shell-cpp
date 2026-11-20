#pragma once

#include <string>
#include <set>
#include <vector>
#include <map>
#include <functional>

class Shell {
public:
    Shell();

    std::vector<std::string> ParseInput();
    bool HandleCommands(std::vector<std::string> commands);
    void Echo(std::string input);
    void Type(std::string input);
    void Path(std::string input);
    std::string PathTo(std::string input);
    bool Execute(std::string command, std::vector<std::string> args);
    void ChangeDir(std::string dirPath);
    void Cat(std::string input);
    void CommandNotFound(std::string input);
    
    void Run();

    void RelativePath(std::string& dirPath);
    void AbsolutePath(std::string& dirPath);

    void ParseQuotes(std::string& input, std::vector<std::string>& commands);
    void ParseSingleQuotes(std::string& input, std::vector<std::string>& commands, std::string& command);
    void ParseDoubleQuotes(std::string& input, std::vector<std::string>& commands, std::string& command);
    void ParseNormal(std::string& input, std::vector<std::string>& commands);
    void ParseBackSlash(std::string& input, std::vector<std::string>& commands);
private:
    std::set<std::string> builtins;
    std::set<std::string> paths;
    std::string currDir;

    std::map<std::string, std::function<void(std::vector<std::string>&)>> commands;
};