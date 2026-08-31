CC = gcc
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++21

TARGET = ccshell

SRCS = \
	src/main.cc

OBJS = $(SRCS:.cc=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
