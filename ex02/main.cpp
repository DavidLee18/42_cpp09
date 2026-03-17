#include "PmergeMe.hpp"
#include <cstddef>
#include <exception>
#include <iostream>

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cerr << "usage: " << argv[0] << " <num1> <num2> ..." << std::endl;
    return 1;
  }
  try {
    PmergeMe merg(static_cast<std::size_t>(argc), argv);
    merg.display_vec();
  } catch (std::exception &e) {
    std::cerr << "operation PmergeMe failed: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
