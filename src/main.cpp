#include <iostream>
#include <string>

void echo()
{
  std::string input;
  getline(std::cin >> std::ws, input);
  std::cout << input << std::endl;
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  while(true)
  {
    std::cout << "$ ";
    std::string command;
    std::cin >> command;
    if(command == "exit")
    {
      break;
    }
    else if(command == "echo")
    {
      echo();
      continue;
    }

    std::cout << command << ": " << "command not found" << std::endl;
  }
}
