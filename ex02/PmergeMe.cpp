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
  size_t whole_len = vec.size(),
         half_len = static_cast<size_t>(std::floor(whole_len / 2));
  std::vector<std::pair<size_t *, size_t *> > chain;
  chain.reserve(half_len);
  for (size_t i = 0; i < half_len; i++) {
    if (vec[2 * i] <= vec[2 * i + 1]) {
      chain.push_back(std::make_pair(&vec[2 * i], &vec[2 * i + 1]));
    } else {
      chain.push_back(std::make_pair(&vec[2 * i], &vec[2 * i + 1]));
    }
  }
  if (chain.size() >= 2) {
    PmergeMe::sort_chains(chain);
  }
  std::vector<size_t> res(whole_len, 0);

  std::vector<std::pair<size_t*,size_t*> >::iterator it = chain.begin();
  for (size_t i = 0; i < half_len && it != chain.end(); i++, it++) {
    for (size_t j = 0; j < whole_len; j++) {
      if (&vec[j] == it->second) {
        res.push_back(vec[j]);
        break;
      }
    }
  }

  for (size_t i = 1;; i++) {
    size_t j = PmergeMe::get_nth_jacobsthal(i);
    it = chain.begin();
    for (size_t k=0;k<j-1&&it!=chain.end();k++) { ++it; }
    for (;j > PmergeMe::get_nth_jacobsthal(i - 1); j--, it--) {
      for (size_t k = 0; k < whole_len; k++) {
        if (&vec[k] == it->first) {
          PmergeMe::binary_insert(res, vec[k], 0, j - 1);
          break;
        }
      }
    }
  }

  if (whole_len % 2 == 1) {
    PmergeMe::binary_insert(res, *vec.end(), 0, res.size()-1);
  }
  vec.swap(res);
}

void PmergeMe::sort_chains(std::vector<std::pair<size_t *, size_t *> > &chain) {
  if (chain.size() < 2) {
    return;
  }
  const size_t whole_len = chain.size(),
               half_len = static_cast<size_t>(std::floor(whole_len / 2));
  std::vector<std::pair<size_t *, size_t *> > subchain;
  subchain.reserve(half_len);
  for (size_t i = 0; i < half_len; i++) {
    if (*(chain[2 * i].second) <= *(chain[2 * i + 1].second)) {
      subchain.push_back(
          std::make_pair(chain[2 * i].second, chain[2 * i + 1].second));
    } else {
      subchain.push_back(
          std::make_pair(chain[2 * i + 1].second, chain[2 * i].second));
    }
  }
  if (subchain.size() >= 2) {
    std::cout << "subchain's size (before): " << subchain.size() << std::endl;
    PmergeMe::sort_chains(subchain);
    std::cout << "subchain's size: " << subchain.size() << std::endl;
  }
  std::vector<std::pair<size_t *, size_t *> > res;
  std::vector<std::pair<size_t*,size_t*> >::iterator it = subchain.begin();
  for (size_t i = 0; i < half_len&&it!=subchain.end(); i++,it++) {
    std::vector<std::pair<size_t*,size_t*> >::iterator jt = chain.begin();
    for (size_t j = 0; j < whole_len&& jt != chain.end(); j++, jt++) {
      if (jt->second == it->second) {
        res.push_back(*jt);
        break;
      }
    }
  }
  for (size_t i = 1;; i++) {
size_t j = PmergeMe::get_nth_jacobsthal(i);
std::vector<std::pair<size_t*,size_t*> >::iterator jt = subchain.begin();
for (size_t k=0;k<j-1&&jt!=subchain.end();k++) { jt++; }
    for (;
         j > PmergeMe::get_nth_jacobsthal(i - 1); j--, jt--) {
           std::vector<std::pair<size_t*,size_t*> >::iterator kt = chain.begin();
      for (size_t k=0; k < whole_len&&kt!=chain.end(); k++, kt++) {
        if (kt->second == jt->first) {
          PmergeMe::binary_insert(res, *kt, 0, static_cast<size_t>(jt - subchain.begin()));
          break;
        }
      }
      if (jt == subchain.begin()) {break;}
    }
    if (PmergeMe::get_nth_jacobsthal(i) >= half_len) {
      break;
    }
  }
  if (whole_len % 2 == 1) {
    it = chain.begin(); for (size_t j = 0;j<whole_len-1&&it!=chain.end();j++) {it++;}
    if (it==chain.end()) {PmergeMe::binary_insert(res, *(--it), 0, chain.size()-1);}
    else {PmergeMe::binary_insert(res, *it, 0, std::min(whole_len-1, chain.size()-1));}
  }
  chain.swap(res);
}

size_t PmergeMe::get_nth_jacobsthal(size_t n) {
  return static_cast<size_t>((std::pow(2, n + 1) + std::pow(-1, n)) / 3);
}

void PmergeMe::binary_insert(std::vector<std::pair<size_t *, size_t *> > &chain,
                             std::pair<size_t *, size_t *> const &item,
                             size_t from, size_t to) {
  const size_t mid = static_cast<size_t>(std::floor((from + to) / 2));
  std::vector<std::pair<size_t*,size_t*> >::iterator it = chain.begin();
  for(size_t i=0;i<mid&&it!=chain.end();i++) {it++;}
  if (it == chain.end()) {
    chain.push_back(item);
  }
  else if (*(it->second) > *(item.second)) {
    if (mid == from) {
      chain.insert(it, item);
    } else {
      PmergeMe::binary_insert(chain, item, from, mid);
    }
  } else {
    if (mid == to) {
      std::vector<std::pair<size_t *, size_t *> >::iterator it = chain.begin();
      for (size_t i = 0; i < to + 1 && it != chain.end(); i++) {
        ++it;
      }
      chain.insert(it, item);
    } else {
      PmergeMe::binary_insert(chain, item, mid, to);
    }
  }
}

void PmergeMe::binary_insert(std::vector<size_t> &vec, size_t const &item,
                             size_t from, size_t to) {
  const size_t mid = static_cast<size_t>(std::floor((from + to) / 2));
  std::vector<size_t>::iterator it = vec.begin();
  for (size_t i=0;i<mid&&it!=vec.end();i++) {it++;}
  if (*it > item) {
    if (mid == from) {
      vec.insert(it, item);
    } else {
      PmergeMe::binary_insert(vec, item, from, mid);
    }
  } else {
    if (mid == to) {
      for (size_t i = mid; i < to + 1 && it != vec.end(); i++) {
        ++it;
      }
      vec.insert(it, item);
    } else {
      PmergeMe::binary_insert(vec, item, mid, to);
    }
  }
}
