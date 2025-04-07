#include "TestList.hh"
#include <cstdlib>
#include <iostream>

// Define ANSI color codes
namespace Color {
const std::string Reset = "\033[0m";
const std::string Red = "\033[31m";
const std::string Green = "\033[32m";
const std::string Yellow = "\033[33m";
} // namespace Color

int main() {
  int failed = 0;
  int total = 0;

  for (const auto &test : testExecutables) {
    total++;
    std::cout << Color::Yellow << "Running " << test << "... " << Color::Reset
              << std::endl;

    // Using system for general portability. Alternative could be boost but
    // thats overengineering.
    int exitCode = std::system(("./" + test).c_str());

    if (exitCode != 0) {
      std::cout << Color::Red << "FAIL" << Color::Reset << std::endl;
      failed++;
    } else {
      std::cout << Color::Green << "PASS" << Color::Reset << std::endl;
    }
  }

  // Summary:
  std::cout << "RESULTS: ";
  if (failed > 0) {
    std::cout << Color::Red << failed << " failed" << Color::Reset;
  } else {
    std::cout << Color::Green << "All passed" << Color::Reset;
  }
  std::cout << " / " << total << " total" << std::endl;

  return failed > 0 ? -1 : 0;
}
