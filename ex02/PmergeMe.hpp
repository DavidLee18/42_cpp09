#ifndef PMERGE_ME_H
#define PMERGE_ME_H

#include <asm/termbits.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>
#include <vector>

class PmergeMe {
  static void binary_insert(std::vector<std::pair<size_t *, size_t *> > &,
                            size_t, size_t); // TODO
  static void sort_chains(std::vector<std::pair<size_t *, size_t *> > &);
  static size_t get_nth_jacobsthal(size_t);

public:
  PmergeMe();
  PmergeMe(PmergeMe const &);
  ~PmergeMe();

  PmergeMe &operator=(PmergeMe const &);
  virtual void phantom() = 0;
  std::pair<std::vector<size_t>, std::list<size_t> >
  parse(size_t, char **) throw(std::invalid_argument);
  static void display_vec(std::vector<size_t> const &,
                          std::string const *const) throw(std::runtime_error);
  static void sort(std::vector<size_t> &);
};

template <typename T>
std::ostringstream &try_print(std::ostringstream &oss, T val) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0) {
    oss << "failed to get terminal window size: ";
    oss << std::strerror(errno);
    throw std::runtime_error(oss.str());
  }
  std::string curr(oss.str());
  oss << val;
  if (oss.str().size() > ws.ws_col) {
    std::cout << curr << std::endl;
    oss.str("");
    oss.clear();
    oss << "    " << val;
  }
  return oss;
}

#endif
