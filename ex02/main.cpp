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
    std::pair<std::vector<size_t>, std::list<size_t> > vec_ls =
        PmergeMe::parse(static_cast<std::size_t>(argc), argv);
    std::string pre("Before: ");
    PmergeMe::display_vec(vec_ls.first, &pre);
    PmergeMe::sort(vec_ls.first);
    pre = "After: ";
    PmergeMe::display_vec(vec_ls.first, &pre);
  } catch (std::exception &e) {
    std::cerr << "operation PmergeMe failed: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
