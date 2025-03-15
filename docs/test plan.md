# Test Plan

## Unit testing:

#### Checking how the cards work:

- Card: creating a card, getting a name, cost.

- UnitCard: checking attack, health reduction.

- SpellCard: use a spell.

#### Checking attack and defense mechanics:

- BattleSystem: resolving attacks, handling damage, applying effects.

#### Checking AI performance:

- AIPlayer: selecting a map, selecting a target, executing a move.

## Integration testing:

#### Testing the interaction between components:
 - GameEngine, Player, AI, BattleSystem.

 - Transfer of moves from player to AI and back.

 - Correctness of game state update after each move.

## System Testing:

#### Complete testing of the game in different scenarios:

  - Start of game, player's turn, AI's turn, end of game.

  - Playing at all AI difficulty levels.
  
  - Testing the handling of exceptional situations (e.g. mana shortage, empty deck).

## Testing Tools

- Google Test