#ifndef PMERGE_ME_H
#define PMERGE_ME_H

#include <cstddef>
#include <list>
#include <stdexcept>
#include <vector>

class PmergeMe {
  std::vector<std::size_t> vec;
  std::list<std::size_t> ls;
public:
  void display_vec() throw(std::runtime_error);


  PmergeMe();
  PmergeMe(size_t, char **) throw(std::invalid_argument);
  PmergeMe(PmergeMe const &);
  ~PmergeMe();

  PmergeMe &operator=(PmergeMe const &);
  void sort() throw(std::runtime_error);
};

#endif
