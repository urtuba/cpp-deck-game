# cpp_deck_game
It is not a game or usefull game tracking app. It's written with learning purpose. It uses <strong>dynamic memmory alocation</strong> to implement <strong>stack</strong>s which is subject of <strong>data structures</strong>

## Gameplay

#### Players And Decks
Unlike traditional cards of standard 52-card deck, card of this game only have a positive or negative number on it. There are 3 decks, 2 for players and 1 in the table. First player starts with taking a card from the deck on the table. If the card's integer is positive, player takes that amount of cards from the competitor. If card says negative, player gives that amount of cards to the competitor. Taking cards are always done from the top most card of decks. Giving cards are done to top of decks, also.  

#### Bin
There is one rule which is not mentioned under in <b>Players and Decks</b> title.  There is a bin under table.  Player takes cards only if the cards to be taken are greater than the current card on top of his own deck. Otherwise cards are thrown away into the bin. 

#### Winner
The winners is the player who can give all the cards in her deck. But this program -although it can be easly modify to this purpose- only cares about number of card in the bin, not about winner. 

## Use

#### Input File (Game File)
First line of input file should contain two positive integers: <b>number of cards in the table deck <em>t</em></b>, whitespace and <b>number of cards in the player's deck<em>p</em></b>. (just one for two players because they starts with same number of cards). Then there are series of lines which contains one integer per one. First <b><em>t</em></b> lines for cards in the table, then <b><em>p</em></b> lines for first player's cards, finally last <b><em>p</em></b> lines for second player's deck.

#### Use
Run terminal / cmdline and go to the containing directory. Run compiler:
<code>
  g++ card_game.cpp -o card_game
</code>
Then give your input file as command line argument and run the code:
<code>
  ./card_game example.game  //for linux
  
  card_game example.game  //for windows
</code>
Then program prints the number of cards in the bin:
<code>
  > g++ card_game.cpp -o card_game
  > card_game example.game
  > 2
 </code>
