CXX ?= c++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2

all: card_game

card_game: card_game.cpp
	$(CXX) $(CXXFLAGS) -o $@ card_game.cpp

test: card_game
	sh tests/run.sh ./card_game

clean:
	rm -f card_game

.PHONY: all test clean
