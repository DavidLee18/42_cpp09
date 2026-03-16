#include "RPN.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>

RPN::RPN() : reg() {}

RPN::RPN(RPN const &other) : reg(other.reg) {}

RPN::~RPN() {}

RPN &RPN::operator=(RPN const &other) {
  if (this != &other) {
    reg = other.reg;
  }
  return *this;
}

unsigned short RPN::calc(std::string const &expr) throw(std::logic_error) {
  unsigned short a = 0, b = 0;
  for (std::string::const_iterator it = expr.begin(); it != expr.end(); it++) {
    if (*it == ' ')
      continue;
    switch (*it) {
    case '-':
      if (reg.size() < 2) {
        std::ostringstream oss;
        oss << "invalid stack size: " << reg.size();
        throw std::logic_error(oss.str());
      }
      a = reg.top();
      reg.pop();
      b = reg.top();
      reg.pop();
      reg.push(b - a);
      break;
    case '+':
      if (reg.size() < 2) {
        std::ostringstream oss;
        oss << "invalid stack size: " << reg.size();
        throw std::logic_error(oss.str());
      }
      a = reg.top();
      reg.pop();
      b = reg.top();
      reg.pop();
      reg.push(a + b);
      break;
    case '*':
      if (reg.size() < 2) {
        std::ostringstream oss;
        oss << "invalid stack size: " << reg.size();
        throw std::logic_error(oss.str());
      }
      a = reg.top();
      reg.pop();
      b = reg.top();
      reg.pop();
      reg.push(a * b);
      break;
    case '/':
      if (reg.size() < 2) {
        std::ostringstream oss;
        oss << "invalid stack size: " << reg.size();
        throw std::logic_error(oss.str());
      }
      a = reg.top();
      reg.pop();
      b = reg.top();
      reg.pop();
      reg.push(b / a);
      break;
    default:
      if (static_cast<bool>(std::isdigit(static_cast<int>(*it)))) {
        reg.push(static_cast<unsigned short>(*it - '0'));
      } else {
        std::ostringstream oss;
        oss << "unknown character " << *it;
        throw std::logic_error(oss.str());
      }
    }
  }
  if (reg.size() != 1) {
    std::ostringstream oss;
    oss << "invalid stack size: " << reg.size();
    throw std::logic_error(oss.str());
  }
  return reg.top();
}
