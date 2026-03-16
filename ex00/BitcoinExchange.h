#ifndef BITCOIN_EXCHANGE_H
#define BITCOIN_EXCHANGE_H

#include <ctime>
#include <map>
#include <string>

class BitcoinExchange {
  std::map<std::time_t, std::string> db;
  public:
    BitcoinExchange();
    BitcoinExchange(std::string const &);
    BitcoinExchange(BitcoinExchange const &);
    ~BitcoinExchange();

    BitcoinExchange &operator=(BitcoinExchange const &);
};

#endif
