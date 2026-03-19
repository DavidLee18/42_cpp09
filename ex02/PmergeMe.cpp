#include "PmergeMe.hpp"
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
  size_t i = 0, j = 0;
  std::vector<PmergeMe::MainChainRef> main_chain(
      static_cast<size_t>(std::floor(vec.size() / 2)));
  for (std::vector<size_t>::iterator it = vec.begin(); it != vec.end();
       it++, i++) {
    if (++it == vec.end()) {
      break;
    }
    main_chain[i] =
        PmergeMe::MainChainRef(*(--it), static_cast<size_t>(it - vec.begin()));
    if (main_chain[i].getMain() > *(++it)) {
      j = main_chain[i].getMain();
      main_chain[i] =
          PmergeMe::MainChainRef(*it, static_cast<size_t>(it - vec.begin()));
      *it = j;
    }
  }
  if (main_chain.size() >= 2) {
    PmergeMe::sort_refs(main_chain);
  }
  // TODO: insert subchain into main chain
}

void PmergeMe::sort_refs(std::vector<MainChainRef> &refs) {
  if (refs.size() < 2) {
    return;
  } else if (refs.size() == 2) {
    if (refs[0] > refs[1]) {
      refs[0].setAfterIdx(refs[1].getBeforeIdx());
      refs[1].setAfterIdx(refs[0].getBeforeIdx());
    }
  } else {
    std::vector<PmergeMe::MainChainRef> chain(
        static_cast<size_t>(std::floor(refs.size() / 2)));
    std::vector<PmergeMe::MainChainRef> remnants;
    std::vector<PmergeMe::MainChainRef>::iterator last(--refs.end());
    size_t i = 0;
    std::vector<PmergeMe::MainChainRef>::iterator it;
    for (it = refs.begin(); it != refs.end(); it++, i++) {
      if (++it == refs.end()) {
        --it;
        break;
      }
      chain[i] = PmergeMe::MainChainRef(*(--it));
      PmergeMe::MainChainRef temp(*(++it));
      if (chain[i].getMain() > it->getMain()) {
        temp = chain[i];
        chain[i] = *it;
        *it = temp;
      }
      remnants.push_back(temp);
    }
    if (it != refs.end()) {}
  }
}
