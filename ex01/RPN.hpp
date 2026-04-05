#ifndef RPN_H
#define RPN_H

#include <stack>
#include <stdexcept>
#include <string>

class RPN {
  std::stack<short> reg;

public:
  RPN();
  RPN(RPN const &);
  ~RPN();

  RPN &operator=(RPN const &);
  short calc(std::string const &) throw(std::logic_error);
};

#endif
