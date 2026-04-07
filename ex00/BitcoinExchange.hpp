#ifndef BITCOIN_EXCHANGE_H
#define BITCOIN_EXCHANGE_H

#include <ctime>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>

class BitcoinExchange {
  std::map<std::time_t, float> db;
  bool invalid;

public:
  BitcoinExchange();
  BitcoinExchange(BitcoinExchange const &);
  ~BitcoinExchange();

  BitcoinExchange &operator=(BitcoinExchange const &);
  bool calcPrice(std::string const &);
};

std::pair<std::pair<struct tm, float>, bool> parse_line(const char *,
                                                        const char *);

std::ostream &operator<<(std::ostream &, struct tm);

#endif
