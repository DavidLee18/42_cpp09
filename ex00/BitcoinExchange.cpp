#include "BitcoinExchange.hpp"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>

BitcoinExchange::BitcoinExchange() throw(std::runtime_error) : db() {
  char buffer[256];
  if (getcwd(buffer, sizeof(buffer)) == NULL) {
    throw std::runtime_error("cannot get PWD: " +
                             std::string(std::strerror(errno)));
  }
  const std::string path(buffer + std::string("/data.csv"));
  std::ifstream input(path.c_str());
  if (input.fail() || !input.is_open()) {
    throw std::runtime_error("opening " + path + " failed");
  }
  std::string line;
  struct tm tm;
  std::memset(&tm, 0, sizeof(tm));
  float val = 0.0f;
  int mon = 0, year = 0;
  while (std::getline(input, line)) {
    if (line == "date,exchange_rate")
      continue;
    if (sscanf(line.c_str(), "%d-%d-%d,%f", &year, &mon, &tm.tm_mday, &val) ==
        4) {
      tm.tm_year = year - 1900;
      tm.tm_mon = mon - 1;
      tm.tm_isdst = -1;
      struct tm otm = tm;
      std::mktime(&tm);
      if (otm.tm_year != tm.tm_year || otm.tm_mon != tm.tm_mon ||
          otm.tm_mday != tm.tm_mday) {
        throw std::runtime_error("invalid date in database");
      }
      std::pair<std::map<std::time_t, float>::iterator, bool> res =
          db.insert(std::make_pair(std::mktime(&tm), val));
      if (!res.second) {
        throw std::runtime_error("map insertion error in database");
      }
    } else {
      throw std::runtime_error("invalid format in database");
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

void BitcoinExchange::calcPrice(std::string const &filePath) {
  char buffer[256];
  if (getcwd(buffer, sizeof(buffer)) == NULL) {
    throw std::runtime_error("cannot get PWD: " +
                             std::string(std::strerror(errno)));
  }
  const std::string path(buffer + std::string("/") + filePath);
  std::ifstream input(path.c_str());
  if (input.fail() || !input.is_open()) {
    throw std::runtime_error("error opening " + path);
  }
  std::string line;
  struct tm tm;
  std::memset(&tm, 0, sizeof(tm));
  float val = 0.0f;
  int mon = 0, year = 0;
  while (std::getline(input, line)) {
    if (line == "date | value")
      continue;
    if (sscanf(line.c_str(), "%d-%d-%d | %f", &year, &mon, &tm.tm_mday, &val) ==
        4) {
      tm.tm_year = year - 1900;
      tm.tm_mon = mon - 1;
      tm.tm_isdst = -1;
      struct tm otm = tm;
      std::mktime(&tm);
      if (otm.tm_year != tm.tm_year || otm.tm_mon != tm.tm_mon ||
          otm.tm_mday != tm.tm_mday) {
        std::cerr << "invalid date: " << year << '-' << std::setw(2)
                  << std::setfill('0') << mon << '-' << std::setw(2)
                  << std::setfill('0') << otm.tm_mday << std::endl;
      } else if (val < 0 || val > 1000) {
        std::cerr << "value out of range: " << val << std::endl;
      } else {
        float scl = NAN;
        const std::time_t t = std::mktime(&tm);
        for (std::map<std::time_t, float>::iterator it = db.begin();
             it != db.end(); it++) {
          if (t < (*it).first) {
            scl = (*(--it)).second;
            break;
          }
        }
        if (std::isnan(scl)) {
          std::cerr << "no value for date: " << year << '-' << std::setw(2)
                    << std::setfill('0') << mon << '-' << std::setw(2)
                    << std::setfill('0') << tm.tm_mday << std::endl;
        } else {
          std::cout << year << '-' << std::setw(2) << std::setfill('0') << mon
                    << '-' << std::setw(2) << std::setfill('0') << tm.tm_mday
                    << " => " << val * scl << std::endl;
        }
      }
    } else {
      std::cerr << "invalid format: " << line << std::endl;
    }
  }
}
