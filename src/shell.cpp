#include <shell.h>

#include <iostream>
#include <sstream>
#include <filesystem>
#include <unistd.h> // access()
#include <cstdlib> // std::system

Shell::Shell()
{
    builtins = {"echo", "exit", "type", "pwd", "cd"};
    paths = {"/bin", "/tmp"};
    currDir = std::filesystem::current_path().string();
    //commands["echo"] = [this](std::vector<std::string>& input) { this->Echo(input); };
}

std::vector<std::string> Shell::ParseInput()
{
    std::string input;
    getline(std::cin, input);
    std::vector<std::string> commands;

    if(input.find("\\") != std::string::npos)
    {
        ParseBackSlash(input, commands);
    }
    else if(input.find("\'") != std::string::npos || input.find("\"") != std::string::npos)
    {
        ParseQuotes(input, commands);
    }
    else
    {
        ParseNormal(input, commands);
    }

    //ParseBackSlash(input, commands);
    
    return commands;
}

bool Shell::HandleCommands(std::vector<std::string> commands)
{
    std::string command = commands[0];
    std::string input;
    bool exit = false;
    if(commands.size() < 2)
    {
        input = "";
    }
    else
    {
        input = commands[1];
    }
    if(command == "exit")
    {
        exit = true;
    } 
    else if(command == "echo")
    {
        Echo(input);
    }
    else if(command == "type")
    {
        Type(input);
    }
    else if(command.find("PATH") != std::string::npos)
    {
        Path(command);
    }
    else if(command == "pwd")
    {
        std::cout << currDir << std::endl;
    }
    else if(command == "cd")
    {
        ChangeDir(input);
    }
    else if(command == "cat")
    {
        Cat(input);
    }
    else
    {
        if(!Execute(command, commands))
            CommandNotFound(command);
    }

    return exit;

}

void Shell::CommandNotFound(std::string command)
{
    std::cout << command << ": " << "command not found" << std::endl;
}

void Shell::Echo(std::string input)
{
    if(input.find("echo") != std::string::npos)
    {
        input = input.substr(5, input.length());
    }
    std::cout << input << std::endl;
}

void Shell::Type(std::string input)
{
    if(builtins.contains(input))
    {
        std::cout << input << " is a shell builtin" << std::endl;
    }
    else
    {
        std::string path = PathTo(input);
        if(path != "")
        {
            std::cout << input << " is " << path << std::endl;
        }
        else
        {
            std::cout << input << ": not found" << std::endl;
        }
    }
}

void Shell::Path(std::string input)
{
    std::string path;
    size_t end = input.find("$PATH");
    if(end != std::string::npos)
    {
        for(int i = 5; i < end; ++i)
        {
            if(input[i] != ':')
            {
                path += input[i];
            }
            else
            {
                paths.insert(path);
                if(path == "/usr/bin")
                {
                    paths.insert("/bin");
                }
                path = "";
            }
        }
    }
}

std::string Shell::PathTo(std::string input)
{
    std::string path = "";
    bool found = false;
    std::set<std::string>::iterator pathItr = paths.begin();
    for(; pathItr != paths.end(); ++pathItr)
    {
        std::filesystem::path currPath = *pathItr;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(currPath))
        {
            if(entry.is_regular_file())
            {
                auto status = entry.status();
                auto permissions = status.permissions();
                if((permissions & (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec)) != std::filesystem::perms::none)
                {
                    std::string fileName = entry.path().filename().string();
                    if(fileName == input)
                    {
                        path = entry.path();
                        if(currPath.string() == "/bin")
                        {
                            path = "/usr" + path;
                        }
                    }
                }
            }
        }
        if(found)
        {
            break;
        }
    }
    return path;
}

bool Shell::Execute(std::string command, std::vector<std::string> args)
{
    std::string executableFile = command;
    std::string path = PathTo(command);
    args.erase(args.begin() + 0);
    if(!path.empty() && access(path.c_str(), X_OK) == 0)
    {
        for(auto arg : args)
        {
            command += " " + arg;
        }
        std::system(command.c_str());
        return true;
    }
    return false;
}

void Shell::ChangeDir(std::string dirPath)
{
    RelativePath(dirPath);
    AbsolutePath(dirPath);
    std::filesystem::path newPath = dirPath;
    if(std::filesystem::is_directory(newPath))
    {
        currDir = newPath;
    }
    else
    {
        std::cout << "cd: " << dirPath << ": No such file or directory" << std::endl;
    }
}
void Shell::Cat(std::string input)
{
    std::string command = "cat " + input;
    std::system(command.c_str());
}
void Shell::Run()
{
    while(true)
    {
        std::cout << "$ ";
        std::vector<std::string> commands = ParseInput();
        bool exit = HandleCommands(commands);
        if(exit)
        {
            break;
        }
    }
}

void Shell::RelativePath(std::string& dirPath)
{
    if(dirPath.starts_with("./"))
    {
        dirPath = currDir + dirPath.substr(1, dirPath.length());
    }
    else if(dirPath.starts_with("../"))
    {
        int count = 0;
        while(!dirPath.empty() && dirPath.find("../") != std::string::npos)
        {
            size_t delim = dirPath.find("../");
            if(delim != std::string::npos)
            {
                count += 1;
                dirPath = dirPath.substr(delim + 3, dirPath.length());
            }
        }
        std::string path = "";
        for(int i = currDir.length() - 1; i >= 0; --i)
        {
            if(currDir[i] == '/')
            {
                --count;
            }
            if(count == 0)
            {
                dirPath = currDir.substr(0, i);
                break;
            }
        }
    }
}

void Shell::AbsolutePath(std::string &dirPath)
{
    if(dirPath == "~")
    {
        dirPath = std::getenv("HOME");
    }
}

void Shell::ParseNormal(std::string &input, std::vector<std::string> &commands)
{
    std::istringstream iss(input);
    std::string command;
    iss >> command;
    std::string parameters = "";
    std::string parameter;
    while(iss >> parameter)
    {
        parameters += parameter + " ";
    }
    if(!parameters.empty())
        parameters = parameters.substr(0, parameters.length() - 1);
    commands = {command, parameters};
}

void Shell::ParseBackSlash(std::string &input, std::vector<std::string>& commands)
{
    std::string newInput = "";
    size_t space = input.find(" ");
    std::string command = "";
    if(space != std::string::npos)
    {
        for(int i = 0; i < space; ++i)
        {
            command += input[i];
        }
        input = input.substr(space + 1, input.length());
        if(command == "cat")
        {
            commands.push_back(command);
            commands.push_back(input);
            return;
        }
    }
    for(int i = 0; i < input.length(); ++i)
    {
        if(input[i] == '\\' && i + 1 < input.length())
        {
            newInput += input[i+1];
            i += 1;
        }
        else
        {
            newInput += input[i];
        }
    }
    input = newInput;
    commands.push_back(command);
    commands.push_back(input);
}

void Shell::ParseQuotes(std::string &input, std::vector<std::string>& commands)
{
    size_t commandDelim = input.find(" ");
    std::string command = "";
    if(commandDelim != std::string::npos)
    {
        for(int i = 0; i < commandDelim; ++i)
        {
            command += input[i];
        }
        commands.push_back(command);
        input = input.substr(commandDelim + 1, input.length());
    }
    if(input.find("\"") != std::string::npos)
    {
        ParseDoubleQuotes(input, commands, command);
    }
    else
    {
        ParseSingleQuotes(input, commands, command);
    }
}

void Shell::ParseSingleQuotes(std::string &input, std::vector<std::string> &commands, std::string& command)
{
    std::string newInput = "";
    std::string currInput = "";
    bool open = false;
    char quote = '\'';
    
    for(int i = 0; i < input.length(); ++i)
    {
        if(input[i] == quote)
        {
            if(open)
            {
                open = false;
                newInput += currInput;
                currInput = "";
            }
            else
            {
                open = true;
            }
            if(command != "cat")
                continue;
        }
        currInput += input[i];
    }
    if(!currInput.empty())
    {
        newInput += currInput;
    }
    if(!newInput.empty())
    {
        input = newInput;
    }
    commands.push_back(input);
}

void Shell::ParseDoubleQuotes(std::string &input, std::vector<std::string> &commands, std::string& command)
{
    std::string newInput = "";
    std::string currInput = "";
    bool open = false;
    char quote = '\"';
    
    for(int i = 0; i < input.length(); ++i)
    {
        if(command == "cat")
        {
            newInput += input[i];
        }
        else
        {
            if(input[i] == quote)
            {
                if(open)
                {
                    open = false;
                    newInput += currInput;
                    if(i + 1 < input.length() && input[i+1] != '\"' && input[i-1] != '\"')
                        currInput = " ";
                    else
                        currInput = "";
                }
                else
                {
                    open = true;
                }

                continue;
            }
            if(open || input[i] != ' ')
                currInput += input[i];
        }
    }
    if(!currInput.empty())
    {
        newInput += currInput;
    }
    if(!newInput.empty())
    {
        input = newInput;
    }
    commands.push_back(input);
    
}
