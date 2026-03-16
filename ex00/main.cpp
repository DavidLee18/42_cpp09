#include "BitcoinExchange.hpp"
#include <exception>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <input file>" << std::endl;
    return 1;
  }
  try {
    BitcoinExchange bitex;
    bitex.calcPrice(argv[1]);
  } catch (std::exception &e) {
    std::cerr << "error trying to calc value: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
