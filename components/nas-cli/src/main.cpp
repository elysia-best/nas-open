#include <iostream>
#include <vector>

#include "nas/cli/app.hpp"

int main(int argc, char** argv) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  nas::cli::App app;
  auto res = app.Run(args);
  if (!res.has_value()) {
    std::cerr << "error: " << res.error().message << '\n';
    return 1;
  }

  std::cout << res.value() << '\n';
  return 0;
}
