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
#include <vector>

class PmergeMe {
  std::vector<std::size_t> vec;
  std::list<std::size_t> ls;

public:
  void display_vec(std::string const *const) throw(std::runtime_error);

  PmergeMe();
  PmergeMe(size_t, char **) throw(std::invalid_argument);
  PmergeMe(PmergeMe const &);
  ~PmergeMe();

  PmergeMe &operator=(PmergeMe const &);
  void sort() throw(std::runtime_error);
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
