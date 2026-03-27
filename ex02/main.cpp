#include <cstddef>
#include <exception>
#include <iostream>

#include "PmergeMe.hpp"

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
        std::cout << vec_ls.first.size() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "operation PmergeMe failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
