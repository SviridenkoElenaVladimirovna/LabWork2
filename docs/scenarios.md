# 2. Usage scenarios

## 2.1 Scenario: Game Start
Prerequisite: The system is loaded, the user has started the game.

Main thread:
1.	The system initializes the gameplay.

2.	Player and opponents (AI) are created.

3.	Decks of cards are shuffled.

4. Player and opponent receive starting cards.

5.	The system displays information about the current state of the game.

6.	The game switches to the player's turn mode.

## 2.2 Scenario: Player's turn

Prerequisite: The game is active, it is now the player's turn.

Main thread:

1. Player receives an additional card from the deck.
2.	The system displays the available cards in the hand.
3. The player selects a card to draw.
4.	If the card requires a target selection, the player selects a target.
5.	The card is drawn and its effect is applied.
6.	Player may repeat steps as long as mana lasts.
7.	The player ends the turn.
8.	The system passes the turn to the opponent.

## 2.3 Scenario: AI move

Prerequisite: The move is passed to the opponent (AI).

Main thread:

1.	The system determines the AI's difficulty level.
2. The AI analyzes its hand and the playing field.
3. The AI selects a card to draw.
4.	If the card requires target selection, the AI selects a target by strategy.
5.	The card is played, its effect is applied.
6.	The AI can repeat steps as long as it has enough mana.
7.	AI ends the turn.
8.	The system passes the turn to the player.

## 2.4 Scenario: Game Over
Prerequisite: One of the players' health has dropped to 0.

Main thread:

1.	The system determines the winner.
2. The history of the game is recorded.
3.	A message about the winner is displayed.
4.	The system ends the game process.