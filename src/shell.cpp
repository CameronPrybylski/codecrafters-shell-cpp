#include <shell.h>

#include <iostream>
#include <sstream>

Shell::Shell()
{
    builtins = {"echo", "exit", "type"};
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