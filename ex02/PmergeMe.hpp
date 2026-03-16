#ifndef PMERGE_ME_H
#define PMERGE_ME_H

#include <cstddef>
#include <list>
#include <vector>

class PmergeMe {
  std::vector<std::size_t> vec;
  std::list<std::size_t> ls;

public:
  PmergeMe();
  PmergeMe(char **);
  PmergeMe(PmergeMe const &);
  ~PmergeMe();

  PmergeMe &operator=(PmergeMe const &);
  void sort();
};

#endif
