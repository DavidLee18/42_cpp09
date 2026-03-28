#include <cstddef>
#include <exception>
#include <iostream>

#include "PmergeMe.hpp"

long long get_us() {
    struct timeval tv;
    ::gettimeofday(&tv, 0);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cerr << "usage: " << argv[0] << " <num1> <num2> ..." << std::endl;
        return 1;
    }
    try {
        std::pair<std::vector<size_t>, std::list<size_t> > vec_ls =
            PmergeMe::parse(static_cast<std::size_t>(argc), argv);
        PmergeMe::display_vec(vec_ls.first, "Before: ");
        const long long vec_start = get_us();
        PmergeMe::sort(vec_ls.first);
        const long long vec_end = get_us();
        const long long ls_start = get_us();
        PmergeMe::sort(vec_ls.second);
        const long long ls_end = get_us();
        PmergeMe::display_vec(vec_ls.first, "After: ");
        std::cout << "Elapsed time to sort " << vec_ls.first.size()
                  << " elements with "
                  << "std::vector: "
                  << static_cast<double>(vec_end - vec_start)
                  << "µs, with std::list: "
                  << static_cast<double>(ls_end - ls_start)
                  << "µs." << std::endl;
    } catch (std::exception &e) {
        std::cerr << "operation PmergeMe failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
