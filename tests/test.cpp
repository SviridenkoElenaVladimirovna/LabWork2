#include <gtest/gtest.h>
#include "Cards/Card.h"
#include "Cards/UnitCard.h"
#include "Cards/SpellCard.h"
#include "Players/HumanPlayer.h"
#include "Players/EasyAI.h"
#include "Core/GameEngine.h"
#include "Cards/HealEffect.h"
#include "Cards/DamageEffect.h"
#include "Players/MediumAI.h"
#include "Players/HardAI.h"
#include "Systems/BattleSystem.h"
#include <memory>

class MockGameEngine : public GameEngine {
public:
    MockGameEngine() : GameEngine() {}

    Player* getCurrentPlayer() const override { return nullptr; }
    Player* getOpponent() const override { return nullptr; }
    void displayGameState() const override {}
};

class CardBasicTests : public ::testing::Test {
protected:
    void SetUp() override {}
};

class CardTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        gameEngine = std::make_shared<MockGameEngine>();
        human = std::make_shared<HumanPlayer>("Human", 30, 10, gameEngine.get());
        easyAI = std::make_shared<EasyAI>("EasyAI", 30, 10, gameEngine.get(), nullptr);
        mediumAI = std::make_shared<MediumAI>("MediumAI", 30, 10, gameEngine.get(), nullptr);

        human->setOpponent(mediumAI.get());
        easyAI->setOpponent(human.get());
        mediumAI->setOpponent(human.get());
    }

    void resetPlayers() {
        human->getBattlefield().clear();
        human->getHandRef().clear();
        human->setMana(10);

        easyAI->getBattlefield().clear();
        easyAI->getHandRef().clear();
        easyAI->setMana(10);

        mediumAI->getBattlefield().clear();
        mediumAI->getHandRef().clear();
        mediumAI->setMana(10);
    }

    std::shared_ptr<MockGameEngine> gameEngine;
    std::shared_ptr<HumanPlayer> human;
    std::shared_ptr<EasyAI> easyAI;
    std::shared_ptr<MediumAI> mediumAI;
};

class HardAITestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        gameEngine = std::make_shared<MockGameEngine>();
        human = std::make_shared<HumanPlayer>("Human", 30, 10, gameEngine.get());
        hardAI = std::make_shared<HardAI>("HardAI", 30, 10, gameEngine.get(), nullptr);

        human->setOpponent(hardAI.get());
        hardAI->setOpponent(human.get());
    }

    void resetPlayers() {
        human->getBattlefield().clear();
        human->getHandRef().clear();
        human->setMana(10);

        hardAI->getBattlefield().clear();
        hardAI->getHandRef().clear();
        hardAI->setMana(10);
    }

    std::shared_ptr<MockGameEngine> gameEngine;
    std::shared_ptr<HumanPlayer> human;
    std::shared_ptr<HardAI> hardAI;
};

TEST_F(CardBasicTests, CardBasicProperties) {
    UnitCard warrior("Warrior", 2, 3, 3, false);

    EXPECT_EQ(warrior.getName(), "Warrior");
    EXPECT_EQ(warrior.getCost(), 2);
    EXPECT_EQ(warrior.getType(), CardType::UNIT);
    EXPECT_FALSE(warrior.isPlayed());

    warrior.setPlayed(true);
    EXPECT_TRUE(warrior.isPlayed());
}

TEST_F(CardBasicTests, SpellCardBasicProperties) {
    SpellCard fireball("Fireball", 4, SpellEffect::DAMAGE, 6);

    EXPECT_EQ(fireball.getName(), "Fireball");
    EXPECT_EQ(fireball.getCost(), 4);
    EXPECT_EQ(fireball.getType(), CardType::SPELL);
    EXPECT_EQ(fireball.getEffect(), SpellEffect::DAMAGE);
    EXPECT_EQ(fireball.getPower(), 6);
}

TEST_F(CardBasicTests, SpellDescriptionCorrect) {
    SpellCard fireball("Fireball", 4, SpellEffect::DAMAGE, 6);
    EXPECT_EQ(fireball.getDescription(), "Fireball [Damage: 6]");

    SpellCard heal("Heal", 3, SpellEffect::HEAL, 5);
    EXPECT_EQ(heal.getDescription(), "Heal [Heal: 5]");

    SpellCard draw("Draw", 2, SpellEffect::DRAW, 2);
    EXPECT_EQ(draw.getDescription(), "Draw [Get the card: 2]");

    SpellCard buff("Buff", 2, SpellEffect::BUFF, 1);
    EXPECT_EQ(buff.getDescription(), "Buff [Amplification: +1 to attack]");
}

TEST_F(CardTestFixture, UnitCardCombat) {
    resetPlayers();

    human->playUnitCard(std::make_unique<UnitCard>("Warrior", 2, 3, 5, false));
    mediumAI->playUnitCard(std::make_unique<UnitCard>("Goblin", 1, 2, 4, false));

    human->endTurn();
    mediumAI->endTurn();
    human->startTurn();

    ASSERT_TRUE(human->getBattlefield()[0]->canAttackNow());
    ASSERT_EQ(mediumAI->getBattlefield()[0]->getHealth(), 4);
    ASSERT_EQ(human->getMana(), 8);

    auto result = human->attackWithUnit(0, 0);

    EXPECT_EQ(mediumAI->getBattlefield()[0]->getHealth(), 1);
    EXPECT_EQ(human->getBattlefield()[0]->getHealth(), 3);
    EXPECT_TRUE(human->getBattlefield()[0]->isExhausted());
    EXPECT_EQ(result.damageDealt, 3);
    EXPECT_EQ(human->getMana(), 6);

    human->playUnitCard(std::make_unique<UnitCard>("Archer", 2, 3, 2, false));

    human->endTurn();
    mediumAI->endTurn();
    human->startTurn();

    ASSERT_EQ(human->getMana(), 4);
    ASSERT_EQ(mediumAI->getHealth(), 30);

    auto heroResult = human->attackWithUnit(1, std::numeric_limits<size_t>::max());

    EXPECT_EQ(mediumAI->getHealth(), 27);
    EXPECT_TRUE(heroResult.attackedHero);
    EXPECT_EQ(heroResult.damageDealt, 3);
    EXPECT_TRUE(human->getBattlefield()[1]->isExhausted());
    EXPECT_EQ(human->getMana(), 2);
}

TEST_F(CardTestFixture, DamageEffectWorks) {
    resetPlayers();
    DamageEffect effect(5);
    UnitCard target("Target", 1, 1, 6, false);

    effect.apply(&target, mediumAI.get());
    EXPECT_EQ(target.getHealth(), 1);
}

TEST_F(CardTestFixture, HealEffectWorks) {
    resetPlayers();
    HealEffect effect(3);
    human->takeDamage(5);

    effect.apply(nullptr, human.get());
    EXPECT_EQ(human->getHealth(), 28);
}

TEST_F(CardTestFixture, PlayerCanPlayUnitCard) {
    resetPlayers();
    auto warrior = std::make_unique<UnitCard>("Warrior", 2, 3, 3, false);
    human->playUnitCard(std::move(warrior));
    EXPECT_EQ(human->getBattlefield().size(), 1);
    EXPECT_EQ(human->getBattlefield()[0]->getName(), "Warrior");
}

TEST_F(CardTestFixture, PlayerCanTakeDamage) {
    resetPlayers();
    human->takeDamage(5);
    EXPECT_EQ(human->getHealth(), 25);
}

TEST_F(CardTestFixture, DamageSpellWorks) {
    resetPlayers();
    SpellCard fireball("Fireball", 4, SpellEffect::DAMAGE, 6);
    human->takeDamage(5);

    fireball.play(mediumAI.get(), human.get());
    EXPECT_EQ(human->getHealth(), 19);
}

TEST_F(CardTestFixture, HealSpellWorks) {
    resetPlayers();
    SpellCard heal("Heal", 3, SpellEffect::HEAL, 5);
    human->takeDamage(10);

    heal.play(human.get(), mediumAI.get());
    EXPECT_EQ(human->getHealth(), 25);
}

TEST_F(CardTestFixture, DrawSpellWorks) {
    resetPlayers();
    human->getDeck().addCard(std::make_unique<UnitCard>("Card1", 1, 1, 1, false));
    human->getDeck().addCard(std::make_unique<UnitCard>("Card2", 1, 1, 1, false));

    SpellCard draw("Draw Cards", 2, SpellEffect::DRAW, 2);
    size_t initialHandSize = human->getHand().getCards().size();

    draw.play(human.get(), mediumAI.get());
    EXPECT_EQ(human->getHand().getCards().size(), initialHandSize + 2);
}

TEST_F(CardTestFixture, BuffSpellWorks) {
    resetPlayers();
    auto warrior = std::make_unique<UnitCard>("Warrior", 2, 3, 3, false);
    int initialAttack = warrior->getAttack();
    human->playUnitCard(std::move(warrior));

    SpellCard buff("Buff", 2, SpellEffect::BUFF, 2);
    buff.play(human.get(), mediumAI.get());

    EXPECT_EQ(human->getBattlefield()[0]->getAttack(), initialAttack + 2);
}

TEST_F(CardTestFixture, SpellCantBePlayedWithoutMana) {
    resetPlayers();
    human->setMana(1);
    ASSERT_EQ(human->getMana(), 1);

    SpellCard expensive("Expensive", 20, SpellEffect::DAMAGE, 10);

    int initialAiHealth = mediumAI->getHealth();
    int initialHumanMana = human->getMana();

    expensive.play(human.get(), mediumAI.get());

    EXPECT_EQ(mediumAI->getHealth(), initialAiHealth);
    EXPECT_EQ(human->getMana(), initialHumanMana);
}

TEST_F(CardTestFixture, HealAboveMaxHealth) {
    resetPlayers();
    HealEffect effect(10);
    human->takeDamage(5);
    effect.apply(nullptr, human.get());
    EXPECT_EQ(human->getHealth(), 30);
}

TEST_F(CardTestFixture, EasyAITests) {
    resetPlayers();

    easyAI->getHandRef().addCard(std::make_unique<UnitCard>("CheapUnit", 2, 2, 2, false));
    easyAI->getHandRef().addCard(std::make_unique<SpellCard>("ExpensiveSpell", 8, SpellEffect::DAMAGE, 5));

    int chosenIndex = -1;

    if (easyAI->hasPlayableCards()) {
        auto playableIndices = easyAI->getHand().getPlayableCardsIndices(easyAI->getMana());
        if (!playableIndices.empty()) {
            chosenIndex = playableIndices[0];
        }
    }

    EXPECT_EQ(chosenIndex, 0);

    easyAI->getHandRef().clear();
    easyAI->getHandRef().addCard(std::make_unique<UnitCard>("TestUnit", 3, 3, 3, false));
    easyAI->setMana(10);

    size_t handSizeBefore = easyAI->getHand().getSize();
    size_t battlefieldSizeBefore = easyAI->getBattlefield().size();

    easyAI->takeTurn();

    EXPECT_EQ(easyAI->getHand().getSize(), handSizeBefore - 1);
    EXPECT_EQ(easyAI->getBattlefield().size(), battlefieldSizeBefore + 1);
}

TEST_F(CardTestFixture, MediumAIAttackBehavior) {
    resetPlayers();

    auto weakUnit = std::make_unique<UnitCard>("WeakUnit", 1, 1, 1, false);
    auto strongUnit = std::make_unique<UnitCard>("StrongUnit", 3, 3, 5, false);
    auto attacker = std::make_unique<UnitCard>("Attacker", 2, 2, 4, false);

    UnitCard* weakPtr = weakUnit.get();
    UnitCard* strongPtr = strongUnit.get();
    UnitCard* attackerPtr = attacker.get();

    human->playUnitCard(std::move(weakUnit));
    human->playUnitCard(std::move(strongUnit));
    mediumAI->playUnitCard(std::move(attacker));

    ASSERT_EQ(human->getBattlefield().size(), 2);
    ASSERT_EQ(mediumAI->getBattlefield().size(), 1);
    ASSERT_EQ(mediumAI->getBattlefield()[0].get(), attackerPtr);

    human->endTurn();
    mediumAI->startTurn();

    ASSERT_FALSE(mediumAI->getBattlefield()[0]->isExhausted());
    ASSERT_TRUE(mediumAI->getBattlefield()[0]->canAttackNow());

    const int initialWeakHealth = human->getBattlefield()[0]->getHealth();
    const int initialStrongHealth = human->getBattlefield()[1]->getHealth();

    mediumAI->takeTurn();

    bool weakUnitDamaged = human->getBattlefield()[0]->getHealth() < initialWeakHealth;
    bool strongUnitDamaged = human->getBattlefield()[1]->getHealth() < initialStrongHealth;

    EXPECT_TRUE(weakUnitDamaged || strongUnitDamaged);
    EXPECT_TRUE(mediumAI->getBattlefield()[0]->isExhausted());
}

TEST_F(CardTestFixture, MediumAIAttacksHeroWhenNoTargets) {
    resetPlayers();

    mediumAI->playUnitCard(std::make_unique<UnitCard>("Attacker", 2, 3, 3, false));

    human->endTurn();
    mediumAI->endTurn();
    mediumAI->startTurn();
    mediumAI->setMana(10);

    int initialHealth = human->getHealth();
    mediumAI->takeTurn();

    EXPECT_LT(human->getHealth(), initialHealth);
}

TEST_F(HardAITestFixture, HardAIAvoidsBadTrades) {
    resetPlayers();

    human->playUnitCard(std::make_unique<UnitCard>("BadTrade", 4, 4, 4, false));
    human->playUnitCard(std::make_unique<UnitCard>("Neutral", 3, 3, 3, false));
    hardAI->playUnitCard(std::make_unique<UnitCard>("Attacker", 3, 3, 3, false));

    hardAI->startTurn();
    hardAI->takeTurn();

    bool badTradeAttacked = false;
    for (const auto& unit : human->getBattlefield()) {
        if (unit->getName() == "BadTrade" && unit->getHealth() < 4) {
            badTradeAttacked = true;
            break;
        }
    }
    EXPECT_FALSE(badTradeAttacked);
}

TEST_F(HardAITestFixture, HardAIPlaysStrongestUnitsFirst) {
    resetPlayers();

    hardAI->getHandRef().addCard(std::make_unique<UnitCard>("StrongUnit", 4, 5, 4, false));
    hardAI->getHandRef().addCard(std::make_unique<UnitCard>("WeakUnit", 1, 1, 1, false));
    hardAI->setMana(10);

    hardAI->takeTurn();

    bool strongUnitPlayed = false;
    for (const auto& unit : hardAI->getBattlefield()) {
        if (unit->getName() == "StrongUnit") {
            strongUnitPlayed = true;
            break;
        }
    }
    EXPECT_TRUE(strongUnitPlayed);
}

TEST_F(HardAITestFixture, HardAIAttacksHeroWhenNoTargets) {
    resetPlayers();

    hardAI->playUnitCard(std::make_unique<UnitCard>("Attacker", 3, 3, 3, false));
    hardAI->startTurn();

    int initialHealth = human->getHealth();
    hardAI->takeTurn();

    EXPECT_LT(human->getHealth(), initialHealth);
}

TEST_F(HardAITestFixture, HardAITakesFullTurn) {
    resetPlayers();

    hardAI->getHandRef().addCard(std::make_unique<UnitCard>("TestUnit", 2, 3, 3, false));
    hardAI->setMana(5);

    size_t handSizeBefore = hardAI->getHand().getSize();
    size_t battlefieldSizeBefore = hardAI->getBattlefield().size();

    hardAI->takeTurn();

    EXPECT_EQ(hardAI->getHand().getSize(), handSizeBefore - 1);
    EXPECT_EQ(hardAI->getBattlefield().size(), battlefieldSizeBefore + 1);
}

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<GameEngine>(false);
    }

    std::unique_ptr<GameEngine> engine;
};

TEST_F(IntegrationTest, GameStateUpdate) {
    engine->addHumanPlayer("Player", 30, 10);
    engine->addAIPlayer(1); 

    auto* player = engine->getPlayers()[0].get();
    auto* ai = engine->getPlayers()[1].get();

    player->setOpponent(ai);
    ai->setOpponent(player);

    player->setMana(10);
    player->setMaxMana(10);

    player->getBattlefield().push_back(std::make_unique<UnitCard>("Warrior", 3, 5, 5, false));
    auto* warrior = player->getBattlefield()[0].get();

    player->startTurn();

    ASSERT_TRUE(warrior->canAttackNow());

    BattleSystem::BattleResult result = player->attackWithUnit(0, std::numeric_limits<size_t>::max());

    EXPECT_TRUE(result.attackedHero);
    EXPECT_EQ(result.damageDealt, 5);
    EXPECT_EQ(ai->getHealth(), 25);
    EXPECT_TRUE(warrior->isExhausted());
    EXPECT_EQ(player->getMana(), 8);
    player->endTurn();

    EXPECT_FALSE(warrior->isExhausted());
}
TEST_F(IntegrationTest, GameInitialization) {
    engine->addHumanPlayer("Player", 30, 1);
    engine->addAIPlayer(1);

    ASSERT_EQ(engine->getPlayers().size(), 2);
    EXPECT_EQ(engine->getPlayers()[0]->getName(), "Player");
    EXPECT_EQ(engine->getPlayers()[1]->getName(), "Easy AI");


    EXPECT_EQ(engine->getPlayers()[1]->getOpponent(), engine->getPlayers()[0].get());

    EXPECT_EQ(engine->getPlayers()[0]->getMana(), 1);
    EXPECT_EQ(engine->getPlayers()[1]->getMana(), 1);
}

TEST_F(IntegrationTest, AIBehavior) {
    engine->addHumanPlayer("Player", 30, 10);
    engine->addAIPlayer(2);
    auto* ai = dynamic_cast<MediumAI*>(engine->getPlayers()[1].get());
    ASSERT_NE(ai, nullptr);
    ai->getHandRef().addCard(std::make_unique<UnitCard>("Warrior", 3, 5, 5, true));
    ai->getHandRef().addCard(std::make_unique<SpellCard>("Fireball", 4, SpellEffect::DAMAGE, 6));

    ai->setMana(10);
    ai->setMaxMana(10);

    ai->setOpponent(engine->getPlayers()[0].get());

    ai->takeTurn();

    bool playedCards = ai->getBattlefield().size() > 0 ||
                       ai->getOpponent()->getHealth() < 30 ||
                       ai->getMana() < 10;
    EXPECT_TRUE(playedCards);
}
TEST_F(IntegrationTest, TurnManagement) {
    engine->addHumanPlayer("Player", 30, 1);
    engine->addAIPlayer(1);
    engine->initializeGame();

    auto* turnManager = engine->getTurnManager();
    ASSERT_NE(turnManager, nullptr);

    const auto& players = engine->getPlayers();

    EXPECT_EQ(turnManager->getCurrentPlayer(), players[0].get());
    EXPECT_EQ(turnManager->getTurnCount(), 1);

    turnManager->endTurn();

    EXPECT_EQ(turnManager->getCurrentPlayer(), players[1].get());
    EXPECT_EQ(turnManager->getTurnCount(), 1);

    turnManager->endTurn();

    EXPECT_EQ(turnManager->getCurrentPlayer(), players[0].get());
    EXPECT_EQ(turnManager->getTurnCount(), 2);

    EXPECT_EQ(players[0]->getMaxMana(), 2);
    EXPECT_EQ(players[0]->getMana(), 2);
    EXPECT_EQ(players[1]->getMaxMana(), 2);
    EXPECT_EQ(players[1]->getMana(), 2);
}

TEST_F(IntegrationTest, GameOverCheck) {
    engine->addHumanPlayer("Player", 1, 10);
    engine->addAIPlayer(1);
    engine->initializeGame();
    engine->getPlayers()[0]->takeDamage(10);

    EXPECT_TRUE(engine->isGameOver());
    EXPECT_TRUE(engine->getPlayers()[0]->isDefeated());
    EXPECT_FALSE(engine->getPlayers()[1]->isDefeated());

    auto* turnManager = engine->getTurnManager();
    turnManager->endTurn();
    EXPECT_TRUE(engine->isGameOver());
}

class SystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<GameEngine>(false);
    }

    std::unique_ptr<GameEngine> engine;
};

TEST_F(SystemTest, FullGameCycle) {
    engine->addHumanPlayer("Player", 30, 10);
    engine->addAIPlayer(1);

    auto* player = engine->getPlayers()[0].get();
    auto* ai = engine->getPlayers()[1].get();

    player->getDeck().clear();
    ai->getDeck().clear();

    player->getDeck().addCard(std::make_unique<UnitCard>("Warrior", 2, 3, 3, false));
    ai->getDeck().addCard(std::make_unique<UnitCard>("Goblin", 1, 2, 2, false));

    engine->initializeGame();

    player->startTurn();
    ASSERT_GE(player->getHand().getCards().size(), 1);

    player->endTurn();
    ai->startTurn();

    EXPECT_TRUE(ai->getBattlefield().size() > 0 ||
                ai->getHand().getCards().size() < 4);
}
TEST_F(SystemTest, AllAILevels) {
    for (int difficulty = 1; difficulty <= 3; ++difficulty) {
        engine = std::make_unique<GameEngine>(false);
        engine->addHumanPlayer("Player", 30, 10);
        engine->addAIPlayer(difficulty);

        auto* ai = engine->getPlayers()[1].get();

        ai->getDeck().clear();
        ai->getDeck().addCard(std::make_unique<UnitCard>("CheapUnit", 1, 1, 1, false));
        ai->getDeck().addCard(std::make_unique<UnitCard>("CheapUnit", 1, 1, 1, false));

        engine->initializeGame();
        ai->startTurn();
        ai->setMaxMana(5);
        ai->setMana(4);

        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        ai->takeTurn();

        std::cout.rdbuf(old);
        std::string output = buffer.str();

        bool cardPlayed = output.find("Spell cast:") != std::string::npos ||
                          output.find("is now on the battlefield!") != std::string::npos ||
                          output.find("A spell is used:") != std::string::npos ||
                          output.find("Played unit:") != std::string::npos ||
                          output.find("plays out") != std::string::npos ||
                          output.find("makes a move") != std::string::npos;

        EXPECT_TRUE(cardPlayed) << "AI level " << difficulty << " did not play a card. Output:\n" << output;
    }
}


TEST_F(SystemTest, WinConditions) {
    engine->addHumanPlayer("Player1", 1, 10);
    engine->addHumanPlayer("Player2", 30, 10);

    auto* player1 = engine->getPlayers()[0].get();
    player1->takeDamage(1);
    EXPECT_TRUE(engine->isGameOver());

    engine = std::make_unique<GameEngine>(false);
    engine->addHumanPlayer("Player1", 30, 10);
    engine->addHumanPlayer("Player2", 30, 10);
    engine->initializeGame();

    auto* player2 = engine->getPlayers()[1].get();
    player2->getDeck().clear();
    player2->getHandRef().clear();

    player2->drawCard();

    EXPECT_TRUE(engine->isGameOver());
}

TEST_F(SystemTest, EdgeCases) {
    engine->addHumanPlayer("Player", 30, 1);
    engine->addAIPlayer(1);
    engine->initializeGame();

    auto* player = engine->getPlayers()[0].get();
    player->getHandRef().addCard(std::make_unique<UnitCard>("Expensive", 10, 5, 5, false));

    size_t handSizeBefore = player->getHand().getCards().size();
    player->playCard(3);
    EXPECT_EQ(player->getHand().getCards().size(), handSizeBefore);

    player->getDeck().clear();
    EXPECT_TRUE(player->getDeck().isEmpty());
    bool drawResult = player->drawCard();
    EXPECT_FALSE(drawResult);

    EXPECT_FALSE(player->canAttack());

    int initialHealth = player->getHealth();
    player->heal(5);
    EXPECT_EQ(player->getHealth(), initialHealth);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
