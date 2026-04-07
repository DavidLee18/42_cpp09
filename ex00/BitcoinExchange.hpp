#ifndef BITCOIN_EXCHANGE_H
#define BITCOIN_EXCHANGE_H

#include <ctime>
#include <map>
#include <ostream>
#include <string>

class BitcoinExchange {
  std::map<std::time_t, float> db;
  std::string error_msg;

public:
  BitcoinExchange();
  BitcoinExchange(BitcoinExchange const &);
  ~BitcoinExchange();

  BitcoinExchange &operator=(BitcoinExchange const &);
  std::string calcPrice(std::string const &);
  std::string const &get_error_msg();
};

std::pair<std::pair<struct tm, float>, bool> parse_line(const char *,
                                                        const char *);

std::ostream &operator<<(std::ostream &, struct tm);

#endif
