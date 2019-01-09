// Author: Samed Kahyaoglu
// Student Num: 150160718

#include <iostream>
#include <fstream>
using namespace std;

int abs(int k) {
  if(k < 0) k = -k;
  return k;
} // absolute value function, I need it when card value is negative.

struct Card {
  int value;
  Card* prev;
};//each cards holds a card below it

struct Deck {
  Card* top ;
  int cardNum;
  void addCard(int xd);
  int dropCard();
  void create();
  void clear();
};

void Deck::clear() {
  Card *p;
  while(top)
  {
    p = top;
    top = top -> prev;
    delete p;
  }
} // deletes decks while program is terminating

int Deck::dropCard() {
  Card* cardPtr;
  int returnVal = top -> value;
  cardPtr = top;
  top = top -> prev;
  delete cardPtr;
  cardNum--;
  return returnVal;
}

void Deck::create() {
  cardNum = 0;
  top = NULL;
}

void Deck::addCard(int xd) {
  Card* newCard;
  newCard = new struct Card;
  newCard -> value = xd;
  newCard -> prev = top;
  top = newCard;
  cardNum++;
}

struct Game {
  Deck* p1;
  Deck* p2;
  Deck* table;
  Deck* bin;
  void initializer(char* filename); // creates decks, reads file and fills them with cards.
  void cleaner();
  void p1gives();
  void p2gives(); // i used 2 functions to change cards between players. they make easy to write gamePlay() func.
  int gamePlay(); // while ends, changes cards between 3 deck: player 1, player 2 and player 3. then returns card number in the bin.
};

int Game::gamePlay()
{
  int cardTaken;

  while (true)
  {
    if((p1->cardNum ==0) || (p2->cardNum ==0) || (table->cardNum ==0)) break;

    cardTaken = table->dropCard();
    if (cardTaken < 0) {
      for (int i = 0; i < abs(cardTaken); i++) {
        if(p1->top == NULL) break; //null means there is no card on deck, so game is over.
        p1gives();
      }
    } else {
      for (int i = 0; i < cardTaken; i++) {
        if(p2->top == NULL) break;
        p2gives();
      }
    }

    if((p1->cardNum ==0) || (p2->cardNum ==0) || (table->cardNum ==0)) break;

    cardTaken = table->dropCard();
    if (cardTaken < 0) {
      for (int i = 0; i < abs(cardTaken); i++) {
        if((p1->top == NULL) || (p2->top == NULL)) break;
        p2gives();
      }
    } else {
      for (int i = 0; i < cardTaken; i++) {
        if((p1->top == NULL) || (p2->top == NULL)) break;
        p1gives();
   } } }
  return (bin -> cardNum);
}

void Game::p1gives()
{
  if(p2 -> top == NULL)
    p2 -> addCard(p1 -> dropCard());
  else if (p1 -> top -> value > p2 -> top -> value)
    p2 -> addCard(p1 -> dropCard());
  else if (p1 -> top -> value <= p2 -> top -> value)
    bin -> addCard(p1 -> dropCard());
}

void Game::p2gives() {
  if (p1 -> top == NULL)
    p1 -> addCard(p2 -> dropCard());
  else if(p2 -> top -> value > p1 -> top -> value)
    p1 -> addCard(p2 -> dropCard());
  else if(p2 -> top -> value <= p1 -> top -> value)
    bin -> addCard(p2 -> dropCard());
}

void Game::cleaner() { 
  p1 -> clear();
  p2 -> clear();
  table -> clear();
  bin -> clear();

  delete p1;
  delete p2;
  delete table;
  delete bin;
} // it means program has finished his job and gives allocated memory back.

void Game::initializer(char* filename)
{
  ifstream myFile(filename);
  int tableDeckCount, playerDeckCount;
  myFile >> tableDeckCount;
  myFile >> playerDeckCount;

  p1 = new struct Deck;
  p1 -> create();
  p2 = new struct Deck;
  p2 -> create();
  table = new struct Deck;
  table -> create();
  bin = new struct Deck;
  bin -> create();

  for (int i = 0; i < tableDeckCount; i++) {
    int x;
    myFile >> x;
    table -> addCard(x);
  }
  for (int i = 0; i < playerDeckCount; i++) {
    int x;
    myFile >> x;
    p1 -> addCard(x);
  }
  for (int i = 0; i < playerDeckCount; i++) {
    int x;
    myFile >> x;
    p2 -> addCard(x);
  }
}

int main(int argc, char *argv[])
{
  if (argc == 2) {
    Game myGame; // Game struct contains all required data itself
    myGame.initializer(argv[1]); // initializing it with file
    cout << myGame.gamePlay() << endl; // playing game
    myGame.cleaner(); //cleaning table :)
  } else {
    cout << "You entered less or more arguments then expected." << endl;
  } // even you will not test with wrong number of arguments, not using argc causes warning) also, being sure everything is OK is good.

  return 0;
}
