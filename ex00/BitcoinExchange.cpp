#include "BitcoinExchange.hpp"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <unistd.h>

BitcoinExchange::BitcoinExchange() : db() {
  char buffer[256];

  if (getcwd(buffer, sizeof(buffer)) == NULL) {
    invalid = true;
    return;
  }

  const std::string path(buffer + std::string("/data.csv"));
  std::ifstream input(path.c_str());

  if (input.fail() || !input.is_open()) {
    invalid = true;
    return;
  }

  std::string line;
  while (std::getline(input, line)) {
    if (line == "date,exchange_rate")
      continue;

    std::pair<std::pair<struct tm, float>, bool> parsed =
        parse_line("%d-%d-%d,%f", line.c_str());
    if (!parsed.second) {
      invalid = true;
      return;
    }

    struct tm tm = parsed.first.first;
    float val = parsed.first.second;

    std::pair<std::map<std::time_t, float>::iterator, bool> res =
        db.insert(std::make_pair(std::mktime(&tm), val));
    if (!res.second) {
      invalid = true;
      return;
    }
  }
}

BitcoinExchange::BitcoinExchange(BitcoinExchange const &other) : db(other.db) {}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange &BitcoinExchange::operator=(BitcoinExchange const &other) {
  if (this != &other) {
    db = other.db;
  }
  return *this;
}

bool BitcoinExchange::calcPrice(std::string const &filePath) {
  char buffer[256];

  if (getcwd(buffer, sizeof(buffer)) == NULL)
    return false;

  const std::string path(buffer + std::string("/") + filePath);
  std::ifstream input(path.c_str());

  if (input.fail() || !input.is_open())
    return false;

  std::string line;

  while (std::getline(input, line)) {
    if (line == "date | value")
      continue;
    std::pair<std::pair<struct tm, float>, bool> parsed =
        parse_line("%d-%d-%d | %f", line.c_str());
    float val = parsed.first.second;
    if (!parsed.second || val < 0 || val > 1000) {
      return false;
    } else {
      float scl = NAN;
      struct tm tm = parsed.first.first;
      const std::time_t t = std::mktime(&tm);

      std::map<std::time_t, float>::iterator it = db.lower_bound(t);
      if (it != db.begin())
        scl = (--it)->second;

      if (std::isnan(scl))
        return false;
      std::cout << tm << " => " << std::fixed << std::setprecision(2)
                << val * scl << std::endl;
    }
  }

  return true;
}

std::pair<std::pair<struct tm, float>, bool> parse_date(const char *fmt,
                                                        const char *str) {
  struct tm tm;
  std::memset(&tm, 0, sizeof(tm));
  float val = 0.0f;
  int mon = 0, year = 0;
  if (sscanf(str, fmt, &year, &mon, &tm.tm_mday, &val) == 4) {
    tm.tm_year = year - 1900;
    tm.tm_mon = mon - 1;
    tm.tm_isdst = -1;
    struct tm otm = tm;
    std::mktime(&tm);

    if (otm.tm_year != tm.tm_year || otm.tm_mon != tm.tm_mon ||
        otm.tm_mday != tm.tm_mday)
      return std::make_pair(std::make_pair((struct tm){}, NAN), false);
    return std::make_pair(std::make_pair(tm, val), true);
  }
  return std::make_pair(std::make_pair((struct tm){}, NAN), false);
}

std::ostream &operator<<(std::ostream &os, struct tm _tm) {
  os << _tm.tm_year - 1900 << '-' << std::setw(2) << std::setfill('0')
     << _tm.tm_mon + 1 << '-' << std::setw(2) << std::setfill('0')
     << _tm.tm_mday;
  return os;
}
