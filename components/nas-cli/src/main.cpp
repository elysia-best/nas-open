#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>

#include "nas/cli/app.hpp"

// Split string by whitespace keeping quoted parts (simple version)
std::vector<std::string> SplitArgs(const std::string& line) {
  std::vector<std::string> args;
  std::istringstream iss(line);
  std::string token;
  while (iss >> token) {
    args.push_back(token);
  }
  return args;
}

void PrintHelp() {
  std::cout << "NAS CLI Management Tool\n\n"
            << "Usage:\n"
            << "  nas-cli [namespace] [entity] [action] [flags]\n\n"
            << "Available Namespaces:\n"
            << "  storage   Manage pool, datasets, iscsi, nfs, partition, etc.\n"
            << "  network   Manage interfaces, addresses, routes.\n"
            << "  monitor   Hardware, processes, SMART, diagnostic reports.\n"
            << "  system    General settings, config export, reboots.\n\n"
            << "Interactive Commands:\n"
            << "  help      Show this help message.\n"
            << "  quit/exit Exit the interactive shell.\n\n"
            << "Examples:\n"
            << "  nas-cli storage pool list --json\n"
            << "  nas-cli storage snapshot create pool/dataset@snap1\n"
            << "  nas-cli > storage pool list\n";
}

int main(int argc, char** argv) {
  nas::cli::App app;

  if (argc == 1) {
    // Interactive Mode
    PrintHelp();
    std::cout << "\nEntering interactive shell...\n";
    
    char* line = nullptr;
    while ((line = readline("nas-cli > ")) != nullptr) {
      if (*line) {
        add_history(line);
        std::string s_line(line);
        auto args = SplitArgs(s_line);
        if (!args.empty()) {
          if (args[0] == "quit" || args[0] == "exit") {
            free(line);
            break;
          }
          if (args[0] == "help") {
            PrintHelp();
          } else {
            auto res = app.Run(args);
            if (!res.has_value()) {
              std::cerr << "error: " << res.error().message << '\n';
            } else {
              std::cout << res.value() << '\n';
            }
          }
        }
      }
      free(line);
    }
    std::cout << "Bye.\n";
    return 0;
  }

  // Non-Interactive Mode
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  if (!args.empty() && (args[0] == "help" || args[0] == "--help" || args[0] == "-h")) {
    PrintHelp();
    return 0;
  }

  auto res = app.Run(args);
  if (!res.has_value()) {
    std::cerr << "error: " << res.error().message << '\n';
    return 1;
  }

  std::cout << res.value() << '\n';
  return 0;
}
