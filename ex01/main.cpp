#include "RPN.hpp"
#include <exception>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <expr>" << std::endl;
    return 1;
  }
  try {
    RPN rpn;
    std::cout << rpn.calc(argv[1]) << std::endl;
  } catch (std::exception &e) {
    std::cerr << "calculation failed: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
