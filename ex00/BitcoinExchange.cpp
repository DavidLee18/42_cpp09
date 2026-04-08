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
    error_msg = "cannot get PWD" + std::string(strerror(errno));
    return;
  }

  const std::string path(buffer + std::string("/data.csv"));
  std::ifstream input(path.c_str());

  if (input.fail() || !input.is_open()) {
    error_msg = "failed to open " + path;
    return;
  }

  std::string line;
  while (std::getline(input, line)) {
    if (line == "date,exchange_rate")
      continue;

    std::pair<std::pair<struct tm, float>, bool> parsed =
        parse_line("%d-%d-%d,%f", line.c_str());
    if (!parsed.second) {
      error_msg = "invalid format: " + line;
      return;
    }

    struct tm tm = parsed.first.first;
    float val = parsed.first.second;

    std::pair<std::map<std::time_t, float>::iterator, bool> res =
        db.insert(std::make_pair(std::mktime(&tm), val));
    if (!res.second) {
      error_msg = "failed to insert";
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

std::string BitcoinExchange::calcPrice(std::string const &filePath) {
  char buffer[256];

  if (getcwd(buffer, sizeof(buffer)) == NULL)
    return "failed to get PWD: " + std::string(strerror(errno));

  const std::string path(buffer + std::string("/") + filePath);
  std::ifstream input(path.c_str());

  if (input.fail() || !input.is_open())
    return "failed to open " + path;

  std::string line;

  while (std::getline(input, line)) {
    if (line == "date | value")
      continue;
    std::pair<std::pair<struct tm, float>, bool> parsed =
        parse_line("%d-%d-%d | %f", line.c_str());
    float val = parsed.first.second;
    if (!parsed.second) {
      std::cerr << "invalid format: " << line << std::endl;
      continue;
    } else if (val < 0 || val > 1000) {
      std::cerr << "value out of range: " << val << std::endl;
      continue;
    } else {
      float scl = NAN;
      struct tm tm = parsed.first.first;
      const std::time_t t = std::mktime(&tm);

      std::map<std::time_t, float>::iterator it = db.lower_bound(t);
      if (it != db.begin())
        scl = (--it)->second;
      else
        scl = it->second;

      if (std::isnan(scl))
        return "cannot calculate NAN";
      std::cout << tm << " => " << std::fixed << std::setprecision(2)
                << val * scl << std::endl;
    }
  }

  return "";
}

std::string const &BitcoinExchange::get_error_msg() { return error_msg; }

std::pair<std::pair<struct tm, float>, bool> parse_line(const char *fmt,
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
  os << _tm.tm_year + 1900 << '-' << std::setw(2) << std::setfill('0')
     << _tm.tm_mon + 1 << '-' << std::setw(2) << std::setfill('0')
     << _tm.tm_mday;
  return os;
}
