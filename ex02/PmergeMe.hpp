#ifndef PMERGE_ME_H
#define PMERGE_ME_H

#ifdef __linux__
#include <asm/termbits.h>
#endif
#include <sys/ioctl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <sys/time.h>
#include <utility>
#include <vector>

class PmergeMe {
  static void sort_chains(std::vector<std::pair<size_t *, size_t *> > &);
  static void sort_chains(std::list<std::pair<size_t *, size_t *> > &);

  static size_t get_nth_jacobsthal(size_t);

public:
  PmergeMe();

  PmergeMe(PmergeMe const &);

  ~PmergeMe();

  PmergeMe &operator=(PmergeMe const &);

  virtual void phantom() = 0;

  static std::pair<std::vector<size_t>, std::list<size_t> >
  parse(size_t, char **) throw(std::invalid_argument);

  static void display_vec(std::vector<size_t> const &,
                          char const *) throw(std::runtime_error);

  static void sort(std::vector<size_t> &);
  static void sort(std::list<size_t> &);

  template <typename T>
  static std::ostringstream &try_print(std::ostringstream &oss, T val) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0) {
      oss << "failed to get terminal window size: ";
      oss << std::strerror(errno);
      throw std::runtime_error(oss.str());
    }
    const std::string curr(oss.str());
    oss << val;
    if (oss.str().size() > ws.ws_col) {
      std::cout << curr << std::endl;
      oss.str("");
      oss.clear();
      oss << "    " << val;
    }
    return oss;
  }

  struct PtrChainLargerCmp {
    const std::pair<size_t *, size_t *> *ptr;
    PtrChainLargerCmp();
    PtrChainLargerCmp(std::pair<size_t *, size_t *> const *);
    bool operator()(const std::pair<size_t *, size_t *> &a,
                    const std::pair<size_t *, size_t *> &b) const;
    bool operator()(const std::pair<size_t *, size_t *> &);
  };

  struct PtrChainSmallerCmp {
    std::pair<size_t *, size_t *> const *ptr;
    PtrChainSmallerCmp(std::pair<size_t *, size_t *> const *);
    bool operator()(std::pair<size_t *, size_t *> const &);
  };
};

#endif
