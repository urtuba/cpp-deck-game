// Author: Samed Kahyaoglu
// Github: urtuba
// Restored: 2026

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

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

// The biggest deck size the game file may ask for.
const int MAX_DECK_SIZE = 100000;

// Thrown when the game file is wrong. The message names the line.
class InputError : public std::runtime_error {
public:
    InputError(int line, const std::string& what)
        : std::runtime_error("line " + std::to_string(line) + ": " + what) {}
};

// Splits the input into words separated by whitespace and remembers the
// line number of each word, so errors can point to it.
class WordReader {
public:
    explicit WordReader(std::istream& in) : in_(in) {}

    // Reads the next word. Returns false when the input has no more words.
    bool next(std::string& word) {
        word.clear();
        int c = get();
        while (c != EOF && std::isspace(c)) c = get();
        if (c == EOF) return false;
        wordLine_ = line_;
        while (c != EOF && !std::isspace(c)) {
            word += static_cast<char>(c);
            c = get();
        }
        return true;
    }

    // Line of the word returned last.
    int wordLine() const { return wordLine_; }

    // Number of the last line of the input (1 for an empty input). Used
    // for errors found at the end of the input.
    int endLine() const {
        return (lastChar_ == '\n' && line_ > 1) ? line_ - 1 : line_;
    }

private:
    std::istream& in_;
    int line_ = 1;      // line of the next character to read
    int wordLine_ = 1;
    int lastChar_ = EOF;

    int get() {
        int c = in_.get();
        if (c == EOF) return EOF;
        lastChar_ = c;
        if (c == '\n') line_++;
        return c;
    }
};

enum class NumberStatus { Ok, NotANumber, OutOfRange };

// Reads a whole number (optional sign, then digits) that fits in an int.
NumberStatus parseInt(const std::string& word, int& value) {
    size_t i = 0;
    bool negative = false;
    if (!word.empty() && (word[0] == '+' || word[0] == '-')) {
        negative = word[0] == '-';
        i = 1;
    }
    if (i == word.size()) return NumberStatus::NotANumber;

    // The magnitude stops growing once it is clearly too big for an int.
    unsigned long long magnitude = 0;
    for (; i < word.size(); i++) {
        if (word[i] < '0' || word[i] > '9') return NumberStatus::NotANumber;
        if (magnitude <= 2147483648ULL)
            magnitude = magnitude * 10 + (word[i] - '0');
    }
    if (magnitude > (negative ? 2147483648ULL : 2147483647ULL))
        return NumberStatus::OutOfRange;
    value = negative ? static_cast<int>(-static_cast<long long>(magnitude))
                     : static_cast<int>(magnitude);
    return NumberStatus::Ok;
}

// A word for an error message. Long words are cut.
std::string shown(const std::string& word) {
    if (word.size() > 20) return "'" + word.substr(0, 20) + "...'";
    return "'" + word + "'";
}

// Two players, the table deck and the bin.
class Game {
public:
    // Reads the game from a stream: the two deck sizes, then the table
    // cards, the cards of player 1 and the cards of player 2.
    // Each card is pushed in file order, so the last one ends up on top.
    // Throws InputError if the stream does not hold a valid game.
    void load(std::istream& in) {
        WordReader reader(in);
        int tableCount = readDeckSize(reader);
        int playerCount = readDeckSize(reader);

        int total = tableCount + 2 * playerCount;
        int cardsRead = 0;
        fill(table_, tableCount, reader, cardsRead, total);
        fill(player1_, playerCount, reader, cardsRead, total);
        fill(player2_, playerCount, reader, cardsRead, total);

        std::string extra;
        if (reader.next(extra))
            throw InputError(reader.wordLine(),
                             "unexpected data after the last card: " + shown(extra));
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

    // Reads one deck size from the header.
    static int readDeckSize(WordReader& reader) {
        std::string word;
        if (!reader.next(word))
            throw InputError(reader.endLine(),
                             "header must have two numbers (table cards and cards per player)");
        int size = 0;
        NumberStatus status = parseInt(word, size);
        if (status == NumberStatus::NotANumber)
            throw InputError(reader.wordLine(),
                             "header must be two whole numbers, got " + shown(word));
        if (status == NumberStatus::OutOfRange || size < 0 || size > MAX_DECK_SIZE)
            throw InputError(reader.wordLine(),
                             "deck size " + shown(word) + " must be between 0 and " +
                                 std::to_string(MAX_DECK_SIZE));
        return size;
    }

    // Reads count cards into a deck. cardsRead and total only serve the
    // error message when the input ends too early.
    static void fill(Deck& deck, int count, WordReader& reader, int& cardsRead, int total) {
        for (int i = 0; i < count; i++) {
            std::string word;
            if (!reader.next(word))
                throw InputError(reader.endLine(),
                                 "expected " + std::to_string(total) + " cards but the file has only " +
                                     std::to_string(cardsRead));
            int value = 0;
            NumberStatus status = parseInt(word, value);
            if (status == NumberStatus::NotANumber)
                throw InputError(reader.wordLine(),
                                 "card must be a whole number, got " + shown(word));
            if (status == NumberStatus::OutOfRange)
                throw InputError(reader.wordLine(),
                                 "card " + shown(word) + " is out of int range");
            deck.push(value);
            cardsRead++;
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
        std::cerr << "usage: card_game GAME_FILE\n";
        return 2;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "error: cannot open " << argv[1] << '\n';
        return 1;
    }
    try {
        Game game;
        game.load(file);
        std::cout << game.play() << '\n';
    } catch (const InputError& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
