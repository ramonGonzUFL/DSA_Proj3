#include "lib.hh"
#include <stdexcept>

int main() {
  const std::string result = get_message();
  if (result != "Hello World!") {
    throw std::runtime_error("Test failed: Expected 'Hello World!', got '" +
                             result + "'");
  }

  return 0;
}
