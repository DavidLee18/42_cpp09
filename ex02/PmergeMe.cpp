#include "PmergeMe.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

PmergeMe::PmergeMe() {}

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

PmergeMe::PmergeMe(PmergeMe const &other) { (void)other; }

PmergeMe::~PmergeMe() {}

PmergeMe &PmergeMe::operator=(PmergeMe const &other) {
  (void)other;
  return *this;
}

void PmergeMe::display_vec(
    std::vector<size_t> const &vec,
    char const *const prelude) throw(std::runtime_error) {
  std::ostringstream oss;
  if (prelude)
    oss << prelude;
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
    if (vec[2 * i] <= vec[2 * i + 1]) {
      chain.push_back(std::make_pair(&vec[2 * i], &vec[2 * i + 1]));
    } else {
      chain.push_back(std::make_pair(&vec[2 * i + 1], &vec[2 * i]));
    }
  }
  if (chain.size() >= 2)
    PmergeMe::sort_chains(chain);
  std::vector<size_t> res;

  for (size_t i = 0; i < chain.size(); i++)
    res.push_back(*chain[i].second);

  res.insert(res.begin(), *chain[0].first);

  size_t last_j;
  size_t i_;
  for (i_ = 1; PmergeMe::get_nth_jacobsthal(i_) <= chain.size(); i_++) {
    const size_t last_last_j = PmergeMe::get_nth_jacobsthal(i_ - 1);
    for (size_t j = (last_j = PmergeMe::get_nth_jacobsthal(i_));
         j > last_last_j; j--) {
      std::vector<size_t>::iterator it = std::lower_bound(
          res.begin(),
          res.begin() + std::min(last_j + last_last_j - 1, res.size()),
          *chain[j - 1].first);
      if (it == res.end())
        res.push_back(*chain[j - 1].first);
      else
        res.insert(it, *chain[j - 1].first);
    }
  }
  for (size_t i = chain.size(), j = PmergeMe::get_nth_jacobsthal(i_ + 1);
       i > last_j; i--) {
    std::vector<size_t>::iterator it = std::lower_bound(
        res.begin(), res.begin() + std::min(j + last_j - 1, res.size()),
        *chain[i - 1].first);
    if (it == res.end())
      res.push_back(*chain[i - 1].first);
    else
      res.insert(it, *chain[i - 1].first);
  }

  if (vec.size() % 2 == 1) {
    std::vector<size_t>::iterator it =
        std::lower_bound(res.begin(), res.end(), vec.back());
    if (it == res.end())
      res.push_back(vec.back());
    else
      res.insert(it, vec.back());
  }
  vec.swap(res);
}

void PmergeMe::sort_chains(std::vector<std::pair<size_t *, size_t *> > &chain) {
  if (chain.size() < 2)
    return;
  std::vector<std::pair<size_t *, size_t *> > sub_chain;
  for (size_t i = 0; i < chain.size() / 2; i++) {
    if (*chain[2 * i].second <= *chain[2 * i + 1].second) {
      sub_chain.push_back(
          std::make_pair(chain[2 * i].second, chain[2 * i + 1].second));
    } else {
      sub_chain.push_back(
          std::make_pair(chain[2 * i + 1].second, chain[2 * i].second));
    }
  }

  if (sub_chain.size() >= 2)
    PmergeMe::sort_chains(sub_chain);
  std::vector<std::pair<size_t *, size_t *> > res;

  for (size_t i = 0; i < sub_chain.size(); ++i) {
    for (size_t j = 0; j < chain.size(); j++) {
      if (chain[j].second == sub_chain[i].second) {
        res.push_back(chain[j]);
        break;
      }
    }
  }

  for (size_t i = 0; i < chain.size(); i++) {
    if (chain[i].second == sub_chain[0].first) {
      res.insert(res.begin(), chain[i]);
      break;
    }
  }

  size_t last_j;
  size_t i_;
  for (i_ = 1; PmergeMe::get_nth_jacobsthal(i_) <= sub_chain.size(); i_++) {
    last_j = PmergeMe::get_nth_jacobsthal(i_);
    const size_t last_last_j = PmergeMe::get_nth_jacobsthal(i_ - 1);
    for (size_t j = last_j; j > last_last_j; j--) {
      for (size_t k = 0; k < chain.size(); k++) {
        if (chain[k].second == sub_chain[j - 1].first) {
          std::vector<std::pair<size_t *, size_t *> >::iterator it =
              std::lower_bound(
                  res.begin(),
                  res.begin() + std::min(last_j + last_last_j - 1, res.size()),
                  chain[k], PmergeMe::PtrChainLargerCmp());
          if (it == res.end())
            res.push_back(chain[k]);
          else
            res.insert(it, chain[k]);
          break;
        }
      }
    }
  }

  for (size_t i = sub_chain.size(), k = PmergeMe::get_nth_jacobsthal(i_ + 1);
       i > last_j; i--) {
    for (size_t j = 0; j < chain.size(); j++) {
      if (chain[j].second == sub_chain[i - 1].first) {
        std::vector<std::pair<size_t *, size_t *> >::iterator it =
            std::lower_bound(res.begin(),
                             res.begin() + std::min(k + last_j - 1, res.size()),
                             chain[j], PmergeMe::PtrChainLargerCmp());
        if (it == res.end())
          res.push_back(chain[j]);
        else
          res.insert(it, chain[j]);
        break;
      }
    }
  }

  if (chain.size() % 2 == 1) {
    std::vector<std::pair<size_t *, size_t *> >::iterator it = std::lower_bound(
        res.begin(), res.end(), chain.back(), PmergeMe::PtrChainLargerCmp());
    if (it == res.end())
      res.push_back(chain.back());
    else
      res.insert(it, chain.back());
  }

  chain.swap(res);
}

size_t PmergeMe::get_nth_jacobsthal(const size_t n) {
  return static_cast<size_t>(((1 << (n + 1)) + (n % 2 == 1 ? -1 : 1)) / 3);
}

// void PmergeMe::sort(std::list<size_t> &ls) {
//     if (ls.size() < 2)
//         return;
//     std::list<std::pair<size_t *, size_t *> > chain;
//     std::list<size_t>::iterator it = ls.begin();
//     for (size_t i = 0; i < ls.size() / 2 && it != ls.end(); i++, ++it) {
//         size_t *prev = &*it;
//         ++it;
//         if (it == ls.end())
//             break;
//         if (*prev <= *it)
//             chain.push_back(std::make_pair(prev, &*it));
//         else
//             chain.push_back(std::make_pair(&*it, prev));
//     }
//     if (chain.size() >= 2)
//         PmergeMe::sort_chains(chain);
//     std::list<size_t> res;
//
//     std::list<std::pair<size_t *, size_t *> >::iterator jt = chain.begin();
//     for (size_t i = 0; i < ls.size() / 2 && i < chain.size(); i++, ++jt)
//         res.push_back(*jt->second);
//
//     res.insert(res.begin(), *chain.begin()->first);
//     size_t last_j = 0;
//     for (size_t i = 1; PmergeMe::get_nth_jacobsthal(i) <= chain.size(); i++)
//     {
//         last_j = PmergeMe::get_nth_jacobsthal(i);
//         jt = chain.begin();
//         for (size_t j = 0; j < last_j; j++, ++jt) {
//         }
//         for (size_t j = last_j; j > PmergeMe::get_nth_jacobsthal(i - 1);
//              j--, --jt) {
//             std::list<size_t>::iterator kt = res.begin();
//             for (size_t k = 0; k < 2 * j - 1 && k < res.size(); k++, ++kt) {
//             }
//             res.insert(std::lower_bound(res.begin(), kt, *jt->first),
//                        *jt->first);
//         }
//     }
//
//     jt = --chain.end();
//     for (size_t i = chain.size(); i > last_j; i--, --jt) {
//         std::list<size_t>::iterator kt = res.begin();
//         for (size_t k = 0; k < 2 * i - 1 && k < res.size(); k++, ++kt) {
//         }
//         res.insert(std::lower_bound(res.begin(), kt, *jt->first),
//         *jt->first);
//     }
//
//     if (ls.size() % 2 == 1)
//         res.insert(std::lower_bound(res.begin(), res.end(), ls.back()),
//                    ls.back());
//     ls.swap(res);
// }
//
// void PmergeMe::sort_chains(std::list<std::pair<size_t *, size_t *> > &chain)
// {
//     if (chain.size() < 2)
//         return;
//     std::list<std::pair<size_t *, size_t *> > subchain;
//     std::list<std::pair<size_t *, size_t *> >::iterator it = chain.begin();
//     for (size_t i = 0; i < chain.size() / 2 && it != chain.end(); i++, ++it)
//     {
//         size_t *prev = it->second;
//         ++it;
//         if (it == chain.end())
//             break;
//         if (*prev <= *it->second)
//             subchain.push_back(std::make_pair(prev, it->second));
//         else
//             subchain.push_back(std::make_pair(it->second, prev));
//     }
//     if (subchain.size() >= 2)
//         PmergeMe::sort_chains(subchain);
//     std::list<std::pair<size_t *, size_t *> > res;
//
//     std::list<std::pair<size_t *, size_t *> >::iterator jt =
//     subchain.begin(); for (size_t i = 0;
//          i < chain.size() / 2 && i < subchain.size() && jt != subchain.end();
//          i++, ++jt) {
//         for (it = chain.begin(); it != chain.end(); ++it) {
//             if (it->second == jt->second) {
//                 res.push_back(*it);
//                 break;
//             }
//         }
//     }
//
//     for (it = chain.begin(); it != chain.end(); ++it) {
//         if (it->second == subchain.begin()->first) {
//             res.insert(
//                 std::lower_bound(res.begin(), ++res.begin(), *it,
//                 VecChainLargerCmp()), *it);
//             break;
//         }
//     }
//
//     size_t last_j = 0;
//     std::list<std::pair<size_t *, size_t *> >::iterator kt;
//     for (size_t i = 1; PmergeMe::get_nth_jacobsthal(i) <= subchain.size();
//          i++) {
//         last_j = PmergeMe::get_nth_jacobsthal(i);
//         jt = subchain.begin();
//         for (size_t j = 0; j < last_j; j++, ++jt) {
//         }
//         for (size_t j = last_j; j > PmergeMe::get_nth_jacobsthal(i - 1);
//              j--, --jt) {
//             for (it = chain.begin(); it != chain.end(); ++it) {
//                 if (it->second == jt->first) {
//                     kt = res.begin();
//                     for (size_t k = 0; k < 2 * j - 1 && kt != res.end();
//                          k++, ++kt) {
//                     }
//                     res.insert(
//                         std::lower_bound(res.begin(), kt, *it,
//                         VecChainLargerCmp()), *it);
//                     break;
//                 }
//             }
//             if (jt == subchain.begin())
//                 break;
//         }
//     }
//
//     jt = --subchain.end();
//     for (size_t i = subchain.size(); i > last_j; i--, --jt) {
//         for (it = chain.begin(); it != chain.end(); ++it) {
//             if (it->second == jt->first) {
//                 kt = res.begin();
//                 for (size_t k = 0; k < 2 * i - 1 && kt != res.end();
//                      k++, ++kt) {
//                 }
//                 res.insert(std::lower_bound(res.begin(), kt, *it,
//                 VecChainLargerCmp()),
//                            *it);
//                 break;
//             }
//         }
//     }
//
//     if (chain.size() % 2 == 1)
//         res.insert(
//             std::lower_bound(res.begin(), res.end(), chain.back(),
//             VecChainLargerCmp()), chain.back());
//     chain.swap(res);
// }

PmergeMe::PtrChainLargerCmp::PtrChainLargerCmp() : ptr(NULL) {}

PmergeMe::PtrChainLargerCmp::PtrChainLargerCmp(
    std::pair<size_t *, size_t *> const *ptr)
    : ptr(ptr) {}

bool PmergeMe::PtrChainLargerCmp::operator()(
    const std::pair<size_t *, size_t *> &a,
    const std::pair<size_t *, size_t *> &b) const {
  return *a.second < *b.second;
}

bool PmergeMe::PtrChainLargerCmp::operator()(
    std::pair<size_t *, size_t *> const &a) {
  return reinterpret_cast<size_t>(ptr->second) <
         reinterpret_cast<size_t>(a.second);
}

PmergeMe::PtrChainSmallerCmp::PtrChainSmallerCmp(
    std::pair<size_t *, size_t *> const *ptr)
    : ptr(ptr) {}

bool PmergeMe::PtrChainSmallerCmp::operator()(
    std::pair<size_t *, size_t *> const &x) {
  return reinterpret_cast<size_t>(ptr->first) <
         reinterpret_cast<size_t>(x.second);
}
