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
  std::istringstream iss(expr);
  std::string tok;
  while (iss >> tok) {
    if (tok.size() != 1) {
	  std::ostringstream oss;
	  oss << "invalid token: " << tok;
	  throw std::logic_error(oss.str());
	}
    if (tok == "-" || tok == "+" || tok == "*" || tok == "/") {
        if (reg.size() < 2) {
          throw std::logic_error("insufficient operands for calculation");
        }
        a = reg.top();
        reg.pop();
        b = reg.top();
        reg.pop();
    }
    unsigned char op = static_cast<unsigned char>(tok[0]);
    switch (op) {
    case '-':
      reg.push(a - b);
      break;
    case '+':
      reg.push(a + b);
      break;
    case '*':
      reg.push(a * b);
      break;
    case '/':
      if (b == 0) {
		throw std::logic_error("division by zero");
	  }
      reg.push(a / b);
      break;
    default:
      if (static_cast<bool>(std::isdigit(static_cast<int>(op)))) {
        reg.push(static_cast<unsigned short>(op - '0'));
      } else {
        std::ostringstream oss;
        oss << "unknown character " << op;
        throw std::logic_error(oss.str());
      }
    }
  }
  if (reg.size() != 1) {
    std::ostringstream oss;
    oss << "invalid stack size: " << reg.size();
    oss << "(multiple results or empty stack)";
    throw std::logic_error(oss.str());
  }
  return reg.top();
}
