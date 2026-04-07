#include "BitcoinExchange.hpp"
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <input file>" << std::endl;
    return 1;
  }
  BitcoinExchange bitex;
  if (!bitex.get_error_msg().empty()) {
    std::cerr << "failed to init " << argv[0] << ": " << bitex.get_error_msg()
              << std::endl;
    return 1;
  }
  std::string err(bitex.calcPrice(argv[1]));
  if (!err.empty()) {
    std::cerr << "failed to calc: " << err << std::endl;
    return 1;
  }
  return 0;
}
