CXX := c++
CXXFLAGS := -Wall -Werror -Wextra -Wconversion -g -std=c++98
NAME := btc

SRCS := BitcoinExchange.cpp main.cpp
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re
