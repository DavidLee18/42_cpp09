#include "PmergeMe.hpp"
#include <algorithm>
#include <asm/termbits.h>
#include <cerrno>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>
#include <vector>

PmergeMe::PmergeMe() {}

std::pair<std::vector<size_t>, std::list<size_t> >
PmergeMe::parse(size_t argc, char **argv) throw(std::invalid_argument) {
  std::vector<size_t> vec(argc - 1);
  std::list<size_t> ls(argc - 1);
  for (size_t i = 1; i < argc; i++) {
    char **argp = argv + i;
    size_t val = std::strtoul(argv[i], argp, 10);
    if (**argp != '\0') {
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

PmergeMe::PmergeMe(PmergeMe const &other) { (void)other; }

PmergeMe::~PmergeMe() {}

PmergeMe &PmergeMe::operator=(PmergeMe const &other) {
  (void)other;
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
  if (oss.str().size() > 0)
    std::cout << oss.str();
  std::cout << std::endl;
}

void PmergeMe::sort(std::vector<size_t> &vec) {
  if (vec.size() < 2) {
    return;
  }
  std::vector<std::pair<size_t *, size_t *> > chain;
  for (size_t i = 0; i < vec.size() / 2; i++) {
    if (vec[2 * i] <= vec[2 * i + 1]) {
      chain.push_back(std::make_pair(&vec[2 * i], &vec[2 * i + 1]));
    } else {
      chain.push_back(std::make_pair(&vec[2 * i], &vec[2 * i + 1]));
    }
  }
  if (chain.size() >= 2) {
    PmergeMe::sort_chains(chain);
  }
  std::vector<size_t> res;

  for (size_t i = 0; i < vec.size() / 2 && i < chain.size(); i++) {
    res.push_back(*(chain[i].second));
  }

  for (size_t i = 1; PmergeMe::get_nth_jacobsthal(i) <= chain.size(); i++) {
    for (size_t j = PmergeMe::get_nth_jacobsthal(i);
         j > PmergeMe::get_nth_jacobsthal(i - 1); j--) {
      std::vector<size_t>::iterator it = std::lower_bound(
          res.begin(), res.begin() + j - 1, *(chain[j - 1].first));
      res.insert(it, *(chain[j - 1].first));
    }
  }

  if (vec.size() % 2 == 1) {
    std::vector<size_t>::iterator it =
        std::lower_bound(res.begin(), res.end() - 1, vec.back());
    res.insert(it, vec.back());
  }
  vec.swap(res);
}

void PmergeMe::sort_chains(std::vector<std::pair<size_t *, size_t *> > &chain) {
  if (chain.size() < 2) {
    return;
  }
  std::vector<std::pair<size_t *, size_t *> > subchain;
  for (size_t i = 0; i < chain.size() / 2; i++) {
    if (*(chain[2 * i].second) <= *(chain[2 * i + 1].second)) {
      subchain.push_back(
          std::make_pair(chain[2 * i].second, chain[2 * i + 1].second));
    } else {
      subchain.push_back(
          std::make_pair(chain[2 * i + 1].second, chain[2 * i].second));
    }
  }
  if (subchain.size() >= 2) {
    PmergeMe::sort_chains(subchain);
  }
  std::vector<std::pair<size_t *, size_t *> > res;
  for (size_t i = 0; i < chain.size() / 2 && i < subchain.size(); i++) {
    for (size_t j = 0; j < chain.size() && j < chain.size(); j++) {
      if (chain[j].second == subchain[i].second) {
        res.push_back(chain[j]);
        break;
      }
    }
  }
  for (size_t i = 1; PmergeMe::get_nth_jacobsthal(i) <= subchain.size(); i++) {
    for (size_t j = PmergeMe::get_nth_jacobsthal(i);
         j > PmergeMe::get_nth_jacobsthal(i - 1); j--) {
      for (size_t k = 0; k < chain.size(); k++) {
        if (chain[k].second == subchain[j - 1].first) {
          std::vector<std::pair<size_t *, size_t *> >::iterator it =
              std::lower_bound(res.begin(),
                               res.begin() + std::min(j - 1, res.size() - 1),
                               chain[k]);
          res.insert(it, chain[k]);
          break;
        }
      }
    }
  }
  if (chain.size() % 2 == 1) {
    std::vector<std::pair<size_t *, size_t *> >::iterator it =
        std::lower_bound(res.begin(), res.end() - 1, chain.back());
    res.insert(it, chain.back());
  }
  chain.swap(res);
}

size_t PmergeMe::get_nth_jacobsthal(size_t n) {
  return static_cast<size_t>((std::pow(2, n + 1) + std::pow(-1, n)) / 3);
}
