## Unit Test Report

## 1. Testing of basic functionality of cards

### 1.1. Testing the `Card` class
Basic properties of all types of cards were successfully tested. The tests confirmed correct operation:
- Obtaining the name, value and type of the card
- Controlling the card state (the “played” state).


### 1.2 Testing `UnitCard`.
Testing of combat characteristics of units confirmed:
- Correct calculation of damage when attacking
- Correct health reduction when taking damage
- Correct operation of the “exhaustion” mechanism after an attack
- State preservation between turns


### 1.3 Testing `SpellCard`.
Testing of different types of spells showed:
- Correct operation of damage spells (accurately inflicting the specified damage)
- Healing spells work correctly (with a maximum health limit)

---

## 2. Testing attack and defense mechanics

### 2.1. Combat system
Testing confirmed:
- Correctness of damage calculation when attacking a unit on a unit
- Correctness of attacking a hero (direct health reduction)
- Effect system working (instant application).


### 2.2. Limitations and special cases
The special cases check showed:
- Correct blocking of card play when mana is low
- Correct limitation of treatment to maximum health
- Expected behavior when reaching zero health
- System performance at extreme parameter values

---

## 3. Testing artificial intelligence

### 3.1. `EasyAI`.
Testing of simple AI confirmed:
- Selection of the first available card to play
- Basic attack logic (given a choice)
- Absence of complex strategic logic
- Correct turn completion

### 3.2. `MediumAI`.
A check of the average AI showed:
- Prioritization of attack on weak enemy units
- Correct transition to attack the hero when there are no targets
- Basic evaluation of the favorability of the exchange
- Logical resource management

### 3.3. `HardAI`.
Testing of the sophisticated AI revealed:
- Avoidance of unprofitable exchanges
- Prioritizing strong cards when casting them
- Optimal use of available mana

## Integration Testing Report

## 1. Testing the interaction of game systems

### 1.1. Initialization of game state
Checking if the initial game setup is correct:
- Creating players (human and AI) with the specified parameters
- Correct role assignment (opponents)
- Initialization of starting mana values

### 1.2. Game engine operation
Testing the basic functions of `GameEngine`:
- Adding players of different types
- Managing gameplay
- Correct storage and provisioning of game objects
- Interacting with the move system

---

## 2. Testing of gameplay

### 2.1. Move mechanics
Checking the system of alternating moves:
- Correct transfer of moves between players
- Increasing the move counter
- Correct mana regeneration at the beginning of the turn
- Resetting the “exhaustion” state of units

### 2.2. Updating game state
Checking system response to game events:
- Changing player health
- Updating the state of cards on the battlefield
- Correct application of effects
---

## 3. Testing victory conditions

### 3.1. Testing game completion
Testing game ending conditions:
- Determining victory when zero health is reached
- Blocking further moves after game completion
- Handling attempts to make a move after the game

---

### 4. Testing interaction with AI

### 4.1. AI behavior within the game
Testing AI integration with game systems:
- Correct execution of AI moves
- Interaction with the card and mana system
- Applying effects and attacks

## System Test Report

## 1. Full game cycle testing

### 1.1. Standard gameplay
Checking the correctness of the basic game scenario:
- Initialization of the game with human player and AI (`FullGameCycle`)
- Allocation of starting cards
- Alternate execution of player moves
- Checking the game state after each move

---

## 2. Testing AI difficulty levels

### 2.1. AI behavior of different levels
Testing the behavior of all AI levels (`AllAILevels`):
- `EasyAI` (level 1): basic use of available cards
- `MediumAI` (level 2): simple strategic decisions
- `HardAI` (level 3): optimal utilization of resources

---

## 3. Testing victory conditions

### 3.1. Standard ending conditions
Testing standard game ending scenarios (`WinConditions`):
- Defeat when reaching zero health
- Defeat when unable to draw a card (empty deck)
- Correct determination of the winner

---

### 4. Testing exceptional situations

### 4.1. Exceptional cases
Testing the handling of non-standard scenarios (`EdgeCases`):
- Attempting to play a card without enough mana
- Actions with an empty deck
- Healing at full health
- Attempting to attack without available units

---

# Test Statistic

Total Tests: 32 | Passed: 32 | Failed: 0

Fixed Issues: Segmentation fault in EasyAI card handling


### Test Categories 

1. Core Card Tests (8)

    - Basic Properties (3)
    - Spell Effects (5)

2. Player Tests (6)

    - Combat (3)
    - Card Play (3)

3. AI Tests (12)

    - EasyAI (4)
    - MediumAI (4)
    - HardAI (4)

4. System Tests (6)

    - Integration (4)
    - Edge Cases (2)

### Fix Details 

* Location: EasyAI::playSelectedCard()
* Problem: Null pointer access
* Solution: Added:

    1. Card null checks
    2. Index validation
    3. Safe type conversion

* Verification: 20+ test runs
