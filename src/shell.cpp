#include <shell.h>

#include <iostream>
#include <sstream>
#include <filesystem>
#include <unistd.h>

Shell::Shell()
{
    builtins = {"echo", "exit", "type"};
    paths = {"/bin", "/tmp"};
}

std::vector<std::string> Shell::ParseInput()
{
    std::string input;
    getline(std::cin, input);
    std::istringstream iss(input);
    std::vector<std::string> commands;
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
    else
    {
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
        PathTo(input);
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
        for(auto p : paths)
        {
            //std::cout << p << std::endl;
        }
    }
}

void Shell::PathTo(std::string input)
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
                std::string fileName = entry.path().filename().string();
                if(fileName == input)
                {
                    path = entry.path();
                    if(currPath.string() == "/bin")
                    {
                        path = "/usr" + path;
                    }
                    if(access(fileName.c_str(), X_OK) == 0)
                    {
                        std::cout << path << std::endl;
                        found = true;
                        break;
                    }
                }
            }
        }
        if(found)
        {
            break;
        }
    }
    if(path != "")
    {
        std::cout << input << " is " << path << std::endl;
    }
    else
    {
        std::cout << input << ": not found" << std::endl;
    }
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