#include "PmergeMe.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

PmergeMe::PmergeMe() : vec(), ls() {}

PmergeMe::PmergeMe(size_t argc, char **argv) throw(std::invalid_argument)
    : vec(argc - 1), ls(argc - 1) {
  for (size_t i = 1; i < argc; i++) {
    char **argp = argv + i;
    size_t val = std::strtoul(argv[i], argp, 10);
    if (**argp != '\0') {
      std::ostringstream oss;
      oss << "parsing to size_t failed: ";
      oss << argv[i];
      throw std::invalid_argument(oss.str());
    }
    vec.push_back(val);
    ls.push_back(val);
  }
}

PmergeMe::PmergeMe(PmergeMe const &other) : vec(other.vec), ls(other.ls) {}

PmergeMe::~PmergeMe() {}

PmergeMe &PmergeMe::operator=(PmergeMe const &other) {
  if (this != &other) {
    vec = other.vec;
    ls = other.ls;
  }
  return *this;
}

void PmergeMe::display_vec() throw(std::runtime_error) {
  struct winsize ws;
  std::ostringstream oss;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0) {
    oss << "failed to get terminal window size: ";
    oss << std::strerror(errno);
    throw std::runtime_error(oss.str());
  }
  oss << "[ ";
  const size_t line_width = static_cast<size_t>(ws.ws_col);
  for (std::vector<size_t>::const_iterator it = vec.begin(); it != vec.end();
       it++) {
    std::string curr(oss.str());
    oss << *it;
    if (oss.str().size() > line_width) {
      std::cout << curr << std::endl;
      oss.str("");
      oss.clear();
      oss << "    " << *it;
    }
    if (++it == vec.end()) {
      std::cout << oss.str() << std::endl << " ]";
    } else {
      oss << ", ";
    }
    --it;
  }
}

void PmergeMe::sort() throw(std::runtime_error) {}
