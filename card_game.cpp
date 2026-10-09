// Author: Samed Kahyaoglu
// Github: urtuba
// Restored: 2026

#include <cstdlib>
#include <fstream>
#include <iostream>

// One card. It points to the card below it in the deck.
struct Card {
    int value;
    Card* below;
};

// A deck is a stack of cards built by hand with a linked list.
// It owns its cards and frees them when it is destroyed.
class Deck {
public:
    Deck() : top_(nullptr), size_(0) {}
    ~Deck() {
        while (top_) {
            Card* doomed = top_;
            top_ = top_->below;
            delete doomed;
        }
    }
    Deck(const Deck&) = delete;
    Deck& operator=(const Deck&) = delete;

    bool empty() const { return top_ == nullptr; }
    int size() const { return size_; }
    int topValue() const { return top_->value; }  // the deck must not be empty

    void push(int value) {
        top_ = new Card{value, top_};
        size_++;
    }

    // Removes the top card and returns its value. The deck must not be empty.
    int pop() {
        Card* doomed = top_;
        int value = doomed->value;
        top_ = doomed->below;
        delete doomed;
        size_--;
        return value;
    }

private:
    Card* top_;
    int size_;
};

// Two players, the table deck and the bin.
class Game {
public:
    // Reads the game from a stream: the two deck sizes, then the table
    // cards, the cards of player 1 and the cards of player 2.
    // Each card is pushed in file order, so the last one ends up on top.
    void load(std::istream& in) {
        int tableCount = 0;
        int playerCount = 0;
        in >> tableCount;
        in >> playerCount;
        fill(table_, tableCount, in);
        fill(player1_, playerCount, in);
        fill(player2_, playerCount, in);
    }

    // Plays until a deck runs out and returns how many cards are in the bin.
    int play() {
        while (true) {
            if (isOver()) break;

            // Turn of player 1.
            int card = table_.pop();
            if (card < 0) {
                // Player 1 gives |card| cards to player 2.
                int count = std::abs(card);
                for (int i = 0; i < count; i++) {
                    if (player1_.empty()) break;
                    give(player1_, player2_);
                }
            } else {
                // Player 1 takes card cards from player 2.
                for (int i = 0; i < card; i++) {
                    if (player2_.empty()) break;
                    give(player2_, player1_);
                }
            }

            if (isOver()) break;

            // Turn of player 2.
            card = table_.pop();
            if (card < 0) {
                // Player 2 gives |card| cards to player 1.
                int count = std::abs(card);
                for (int i = 0; i < count; i++) {
                    if (player1_.empty() || player2_.empty()) break;
                    give(player2_, player1_);
                }
            } else {
                // Player 2 takes card cards from player 1.
                for (int i = 0; i < card; i++) {
                    if (player1_.empty() || player2_.empty()) break;
                    give(player1_, player2_);
                }
            }
        }
        return bin_.size();
    }

private:
    Deck player1_;
    Deck player2_;
    Deck table_;
    Deck bin_;

    static void fill(Deck& deck, int count, std::istream& in) {
        for (int i = 0; i < count; i++) {
            int value = 0;
            in >> value;
            deck.push(value);
        }
    }

    // The game ends when the table or one player has no cards left.
    bool isOver() const {
        return player1_.empty() || player2_.empty() || table_.empty();
    }

    // Moves the top card of "from" onto "to" if it is greater than the top
    // card of "to" (or "to" is empty). Otherwise it goes to the bin.
    void give(Deck& from, Deck& to) {
        if (to.empty() || from.topValue() > to.topValue())
            to.push(from.pop());
        else
            bin_.push(from.pop());
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "You entered less or more arguments then expected." << '\n';
        return 0;
    }
    std::ifstream file(argv[1]);
    Game game;
    game.load(file);
    std::cout << game.play() << '\n';
    return 0;
}
