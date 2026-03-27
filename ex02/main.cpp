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
        PmergeMe::display_vec(vec_ls.first, "Before: ");
        const std::clock_t vec_start = std::clock();
        PmergeMe::sort(vec_ls.first);
        const std::clock_t vec_end = std::clock();
        const std::clock_t ls_start = std::clock();
        PmergeMe::sort(vec_ls.second);
        const std::clock_t ls_end = std::clock();
        PmergeMe::display_vec(vec_ls.first, "After: ");
        std::cout << "Elapsed time to sort " << vec_ls.first.size()
                  << " elements with "
                  << "std::vector: "
                  << static_cast<double>(vec_end - vec_start) /
                         (CLOCKS_PER_SEC / 1000000)
                  << "µs, with std::list: "
                  << static_cast<double>(ls_end - ls_start) /
                         (CLOCKS_PER_SEC / 1000000)
                  << "µs." << std::endl;
    } catch (std::exception &e) {
        std::cerr << "operation PmergeMe failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
