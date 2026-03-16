#ifndef BITCOIN_EXCHANGE_H
#define BITCOIN_EXCHANGE_H

#include <ctime>
#include <map>
#include <stdexcept>
#include <string>

class BitcoinExchange {
  std::map<std::time_t, float> db;

public:
  BitcoinExchange() throw(std::runtime_error);
  BitcoinExchange(BitcoinExchange const &);
  ~BitcoinExchange();

  BitcoinExchange &operator=(BitcoinExchange const &);
  void calcPrice(std::string const &);
};

#endif
