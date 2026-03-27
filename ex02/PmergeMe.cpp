#include "PmergeMe.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

PmergeMe::PmergeMe() {
}

std::pair<std::vector<size_t>, std::list<size_t> >
PmergeMe::parse(const size_t argc, char **argv) throw(std::invalid_argument) {
    std::vector<size_t> vec(argc - 1);
    std::list<size_t> ls;
    for (size_t i = 1; i < argc; i++) {
        char *endp = NULL;
        size_t val = std::strtoul(argv[i], &endp, 10);
        if (endp == argv[i] || *endp != '\0') {
            std::ostringstream oss;
            oss << "parsing to size_t failed: ";
            oss << argv[i];
            throw std::invalid_argument(oss.str());
        }
        vec[i - 1] = val;
        ls.push_back(val);
    }
    return std::make_pair(vec, ls);
}

PmergeMe::PmergeMe(PmergeMe const &other) { (void) other; }

PmergeMe::~PmergeMe() {
}

PmergeMe &PmergeMe::operator=(PmergeMe const &other) {
    (void) other;
    return *this;
}

void PmergeMe::display_vec(
    std::vector<size_t> const &vec,
    std::string const *const prelude) throw(std::runtime_error) {
    std::ostringstream oss;
    if (prelude)
        oss << *prelude;
    oss << "[ ";
    for (std::vector<size_t>::const_iterator it = vec.begin(); it != vec.end();
         ++it) {
        try_print(oss, *it);
        try_print(oss, ++it == vec.end() ? " ]" : ", ");
        --it;
    }
    if (!oss.str().empty())
        std::cout << oss.str();
    std::cout << std::endl;
}

void PmergeMe::sort(std::vector<size_t> &vec) {
    if (vec.size() < 2)
        return;
    std::vector<std::pair<size_t *, size_t *> > chain;
    for (size_t i = 0; i < vec.size() / 2; i++) {
        if (vec[2 * i] <= vec[2 * i + 1])
            chain.push_back(std::make_pair(&vec[2 * i], &vec[2 * i + 1]));
        else
            chain.push_back(std::make_pair(&vec[2 * i + 1], &vec[2 * i]));
    }
    if (chain.size() >= 2)
        PmergeMe::sort_chains(chain);
    std::vector<size_t> res;

    for (size_t i = 0; i < vec.size() / 2 && i < chain.size(); i++)
        res.push_back(*(chain[i].second));

    res.insert(res.begin(), *chain[0].first);
    size_t last_j = 0;
    for (size_t i = 1; PmergeMe::get_nth_jacobsthal(i) <= chain.size(); i++) {
        last_j = PmergeMe::get_nth_jacobsthal(i);
        for (size_t j = last_j; j > PmergeMe::get_nth_jacobsthal(i - 1); j--)
            res.insert(
                std::lower_bound(
                    res.begin(),
                    res.begin() +
                    static_cast<std::vector<size_t>::difference_type>(
                        std::min(2 * j - 1, res.size())),
                    *chain[j - 1].first),
                *chain[j - 1].first);
    }
    for (size_t i = chain.size(); i > last_j; i--)
        res.insert(
            std::lower_bound(
                res.begin(),
                res.begin() + static_cast<std::vector<size_t>::difference_type>(
                    std::min(2 * i - 1, res.size())),
                *chain[i - 1].first),
            *chain[i - 1].first);

    if (vec.size() % 2 == 1)
        res.insert(std::lower_bound(res.begin(), res.end(), vec.back()),
                   vec.back());
    vec.swap(res);
}

void PmergeMe::sort_chains(std::vector<std::pair<size_t *, size_t *> > &chain) {
    if (chain.size() < 2)
        return;
    std::vector<std::pair<size_t *, size_t *> > subchain;
    for (size_t i = 0; i < chain.size() / 2; i++) {
        if (*chain[2 * i].second <= *chain[2 * i + 1].second)
            subchain.push_back(
                std::make_pair(chain[2 * i].second, chain[2 * i + 1].second));
        else
            subchain.push_back(
                std::make_pair(chain[2 * i + 1].second, chain[2 * i].second));
    }
    if (subchain.size() >= 2)
        PmergeMe::sort_chains(subchain);
    std::vector<std::pair<size_t *, size_t *> > res;
    for (size_t i = 0; i < chain.size() / 2 && i < subchain.size(); i++) {
        for (size_t j = 0; j < chain.size(); j++) {
            if (chain[j].second == subchain[i].second) {
                res.push_back(chain[j]);
                break;
            }
        }
    }

    for (size_t i = 0; i < chain.size(); i++) {
        if (chain[i].second == subchain[0].first) {
            res.insert(
                std::lower_bound(
                    res.begin(),
                    res.begin() +
                    static_cast<std::vector<
                        std::pair<size_t *, size_t *> >::difference_type>(
                        1),
                    chain[i], ChainCmp()),
                chain[i]);
            break;
        }
    }

    size_t last_j = 0;
    for (size_t i = 1; PmergeMe::get_nth_jacobsthal(i) <= subchain.size();
         i++) {
        last_j = PmergeMe::get_nth_jacobsthal(i);
        for (size_t j = last_j; j > PmergeMe::get_nth_jacobsthal(i - 1); j--) {
            for (size_t k = 0; k < chain.size(); k++) {
                if (chain[k].second == subchain[j - 1].first) {
                    res.insert(
                        std::lower_bound(
                            res.begin(),
                            res.begin() +
                            static_cast<
                                std::vector<size_t>::difference_type>(
                                std::min(2 * j - 1, res.size())),
                            chain[k], ChainCmp()),
                        chain[k]);
                    break;
                }
            }
        }
    }

    for (size_t i = subchain.size(); i > last_j; i--) {
        for (size_t k = 0; k < chain.size(); k++) {
            if (chain[k].second == subchain[i - 1].first) {
                res.insert(
                    std::lower_bound(
                        res.begin(),
                        res.begin() +
                        static_cast<std::vector<size_t>::difference_type>(
                            std::min(2 * i - 1, res.size())),
                        chain[k], ChainCmp()),
                    chain[k]);
                break;
            }
        }
    }

    if (chain.size() % 2 == 1)
        res.insert(
            std::lower_bound(res.begin(), res.end(), chain.back(), ChainCmp()),
            chain.back());
    chain.swap(res);
}

size_t PmergeMe::get_nth_jacobsthal(const size_t n) {
    return static_cast<size_t>(((1 << (n + 1)) + (n % 2 == 1 ? -1 : 1)) / 3);
}

bool PmergeMe::ChainCmp::operator()(
    const std::pair<size_t *, size_t *> &a,
    const std::pair<size_t *, size_t *> &b) const {
    return *a.second < *b.second;
}
