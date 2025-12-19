#include "../include/npc/npc.h"
#include "../include/npc/druid.h"
#include "../include/npc/orc.h"
#include "../include/npc/squirrel.h"
#include "../include/geometry/point.h"
#include <gtest/gtest.h>
#include <memory>


TEST(DruidTest, ConstructorAndType) {
    Point position(5, 5);
    Druid druid("Лабуба", position);
    
    EXPECT_EQ(druid.get_name(), "Лабуба");
    EXPECT_EQ(druid.get_type(), "Друид");
    EXPECT_TRUE(druid.is_alive());
    EXPECT_EQ(druid.get_position().get_x(), 5);
    EXPECT_EQ(druid.get_position().get_y(), 5);
}

TEST(DruidTest, KillMethod) {
    Druid druid("Друид", Point(0, 0));
    EXPECT_TRUE(druid.is_alive());
    
    druid.kill();
    EXPECT_FALSE(druid.is_alive());
}

TEST(DruidTest, VSMethodKillsSquirrel) {
    Druid druid("Друид", Point(0, 0));
    Squirrel squirrel("Белка", Point(1, 1));
    
    auto result = druid.vs(squirrel);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "Друид уничтожил Белку!");
}

TEST(DruidTest, VSMethodAgainstOrc) {
    Druid druid("Друид", Point(0, 0));
    Orc orc("Орк", Point(2, 2));
    
    // Друиды не убивают Орков
    EXPECT_FALSE(druid.vs(orc).has_value());
}

TEST(DruidTest, VSMethodAgainstDruid) {
    Druid druid1("Друид1", Point(0, 0));
    Druid druid2("Друид2", Point(1, 1));
    
    // Друиды не убивают друг друга
    EXPECT_FALSE(druid1.vs(druid2).has_value());
}

TEST(DruidTest, VSMethodWhenDead) {
    Druid druid("Друид", Point(0, 0));
    Squirrel squirrel("Белка", Point(1, 1));
    
    druid.kill();
    
    EXPECT_FALSE(druid.vs(squirrel).has_value());
}

TEST(DruidTest, VSMethodAgainstDeadSquirrel) {
    Druid druid("Друид", Point(0, 0));
    Squirrel squirrel("Белка", Point(1, 1));
    
    squirrel.kill();
    
    EXPECT_FALSE(druid.vs(squirrel).has_value());
}


TEST(OrcTest, ConstructorAndType) {
    Point position(15, 25);
    Orc orc("Баобаб", position);
    
    EXPECT_EQ(orc.get_name(), "Баобаб");
    EXPECT_EQ(orc.get_type(), "Орк");
    EXPECT_TRUE(orc.is_alive());
}

TEST(OrcTest, VSMethodAgainstDruid) {
    Orc orc("Орк", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    
    auto result = orc.vs(druid);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "Орк разорвал бедолагу Друида!");
}

TEST(OrcTest, VSMethodAgainstSquirrel) {
    Orc orc("Орк", Point(0, 0));
    Squirrel squirrel("Белка", Point(1, 1));
    
    // Орки не убивают Белок
    EXPECT_FALSE(orc.vs(squirrel).has_value());
}

TEST(OrcTest, VSMethodAgainstOrc) {
    Orc orc1("Орк1", Point(0, 0));
    Orc orc2("Орк2", Point(1, 1));
    
    // Орки не убивают друг друга
    EXPECT_FALSE(orc1.vs(orc2).has_value());
}

TEST(OrcTest, VSMethodWhenDead) {
    Orc orc("Орк", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    
    orc.kill();
    
    EXPECT_FALSE(orc.vs(druid).has_value());
}

TEST(OrcTest, VSMethodAgainstDeadTarget) {
    Orc orc("Орк", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    
    druid.kill();
    
    EXPECT_FALSE(orc.vs(druid).has_value());
}

TEST(SquirrelTest, ConstructorAndType) {
    Point position(30, 40);
    Squirrel squirrel("Белка", position);
    
    EXPECT_EQ(squirrel.get_name(), "Белка");
    EXPECT_EQ(squirrel.get_type(), "Белка");
    EXPECT_TRUE(squirrel.is_alive());
}

TEST(SquirrelTest, VSMethodNeverAttacks) {
    Squirrel squirrel("Белка", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    Orc orc("Орк", Point(2, 2));
    Squirrel squirrel2("Белка2", Point(3, 3));
    
    // Белки за мир - они никого не убивают
    EXPECT_FALSE(squirrel.vs(druid).has_value());
    EXPECT_FALSE(squirrel.vs(orc).has_value());
    EXPECT_FALSE(squirrel.vs(squirrel2).has_value());
}

TEST(SquirrelTest, VSMethodWhenDead) {
    Squirrel squirrel("Белка", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    
    squirrel.kill();
    
    EXPECT_FALSE(squirrel.vs(druid).has_value());
}

TEST(SquirrelTest, VSMethodAgainstDeadTarget) {
    Squirrel squirrel("Белка", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    
    druid.kill();
    
    EXPECT_FALSE(squirrel.vs(druid).has_value());
}

TEST(PolymorphismTest, GetTypePolymorphism) {
    std::unique_ptr<NPC> druid = std::make_unique<Druid>("Друид", Point(0, 0));
    std::unique_ptr<NPC> orc = std::make_unique<Orc>("Орк", Point(1, 1));
    std::unique_ptr<NPC> squirrel = std::make_unique<Squirrel>("Белка", Point(2, 2));
    
    EXPECT_EQ(druid->get_type(), "Друид");
    EXPECT_EQ(orc->get_type(), "Орк");
    EXPECT_EQ(squirrel->get_type(), "Белка");
}

TEST(PolymorphismTest, BaseClassMethods) {
    std::unique_ptr<NPC> druid = std::make_unique<Druid>("Друид", Point(10, 20));
    
    EXPECT_EQ(druid->get_name(), "Друид");
    EXPECT_EQ(druid->get_position().get_x(), 10);
    EXPECT_EQ(druid->get_position().get_y(), 20);
    EXPECT_TRUE(druid->is_alive());
    
    druid->kill();
    EXPECT_FALSE(druid->is_alive());
}

TEST(InteractionTest, AllVSCombinationsAliveNPCs) {
    Druid druid("Друид", Point(0, 0));
    Orc orc("Орк", Point(1, 1));
    Squirrel squirrel("Белка", Point(2, 2));
    
    EXPECT_TRUE(druid.is_alive());
    EXPECT_TRUE(orc.is_alive());
    EXPECT_TRUE(squirrel.is_alive());
    
    // Друид убивает Белку
    EXPECT_TRUE(druid.vs(squirrel).has_value());
    // Друид не убивает Орка
    EXPECT_FALSE(druid.vs(orc).has_value());
    // Друид не убивает Друида
    EXPECT_FALSE(druid.vs(druid).has_value());
    
    // Орк убивает Друида
    EXPECT_TRUE(orc.vs(druid).has_value());
    // Орк не убивает Белку
    EXPECT_FALSE(orc.vs(squirrel).has_value());
    // Орк не убивает Орка
    EXPECT_FALSE(orc.vs(orc).has_value());
    
    // Белка никого не убивает (за мир)
    EXPECT_FALSE(squirrel.vs(druid).has_value());
    EXPECT_FALSE(squirrel.vs(orc).has_value());
    EXPECT_FALSE(squirrel.vs(squirrel).has_value());
}

TEST(InteractionTest, NoInteractionsWhenDead) {
    Druid druid("Друид", Point(0, 0));
    Orc orc("Орк", Point(1, 1));
    Squirrel squirrel("Белка", Point(2, 2));
    
    druid.kill();
    orc.kill();
    squirrel.kill();
    
    EXPECT_FALSE(druid.vs(orc).has_value());
    EXPECT_FALSE(druid.vs(squirrel).has_value());
    EXPECT_FALSE(druid.vs(druid).has_value());
    
    EXPECT_FALSE(orc.vs(druid).has_value());
    EXPECT_FALSE(orc.vs(squirrel).has_value());
    EXPECT_FALSE(orc.vs(orc).has_value());
    
    EXPECT_FALSE(squirrel.vs(druid).has_value());
    EXPECT_FALSE(squirrel.vs(orc).has_value());
    EXPECT_FALSE(squirrel.vs(squirrel).has_value());
}

TEST(InteractionTest, CombatRules) {
    Druid druid("Друид", Point(0, 0));
    Orc orc("Орк", Point(1, 1));
    Squirrel squirrel("Белка", Point(2, 2));
    
    // Орки убивают Друидов
    EXPECT_TRUE(orc.vs(druid).has_value());
    EXPECT_FALSE(druid.vs(orc).has_value());
    
    // Друиды убивают Белок
    EXPECT_TRUE(druid.vs(squirrel).has_value());
    EXPECT_FALSE(squirrel.vs(druid).has_value());
    
    // Белки за мир - никого не убивают
    EXPECT_FALSE(squirrel.vs(orc).has_value());
    EXPECT_FALSE(squirrel.vs(druid).has_value());
    EXPECT_FALSE(squirrel.vs(squirrel).has_value());
}

TEST(PositionTest, DifferentPositions) {
    Druid druid("Друид", Point(-10, -10));
    Orc orc("Орк", Point(100, 50));
    Squirrel squirrel("Белка", Point(0, -5));
    
    EXPECT_EQ(druid.get_position().get_x(), -10);
    EXPECT_EQ(druid.get_position().get_y(), -10);
    
    EXPECT_EQ(orc.get_position().get_x(), 100);
    EXPECT_EQ(orc.get_position().get_y(), 50);
    
    EXPECT_EQ(squirrel.get_position().get_x(), 0);
    EXPECT_EQ(squirrel.get_position().get_y(), -5);
}

TEST(StateTest, ImmutabilityAfterVS) {
    Druid druid("Друид", Point(0, 0));
    Orc orc("Орк", Point(1, 1));
    
    std::string originalDruidName = druid.get_name();
    Point originalDruidPos = druid.get_position();
    bool originalDruidAlive = druid.is_alive();

    auto result = orc.vs(druid);

    EXPECT_EQ(druid.get_name(), originalDruidName);
    EXPECT_EQ(druid.get_position().get_x(), originalDruidPos.get_x());
    EXPECT_EQ(druid.get_position().get_y(), originalDruidPos.get_y());
    EXPECT_EQ(druid.is_alive(), originalDruidAlive);
    EXPECT_TRUE(result.has_value());
}

TEST(LifeCycleTest, AliveStateManagement) {
    Orc orc("Орк", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    
    EXPECT_TRUE(orc.is_alive());
    EXPECT_TRUE(druid.is_alive());
    
    EXPECT_TRUE(orc.vs(druid).has_value());
    druid.kill();
    EXPECT_FALSE(druid.is_alive());
    
    EXPECT_FALSE(orc.vs(druid).has_value());
    
    orc.kill();
    EXPECT_FALSE(orc.is_alive());
    
    EXPECT_FALSE(orc.vs(druid).has_value());
}

TEST(EdgeCasesTest, EmptyName) {
    Druid druid("", Point(0, 0));
    EXPECT_EQ(druid.get_name(), "");
    EXPECT_EQ(druid.get_type(), "Друид");
}

TEST(EdgeCasesTest, SamePosition) {
    Point samePos(7, 7);
    Druid druid("Друид", samePos);
    Orc orc("Орк", samePos);
    
    EXPECT_EQ(druid.get_position().get_x(), 7);
    EXPECT_EQ(druid.get_position().get_y(), 7);
    EXPECT_EQ(orc.get_position().get_x(), 7);
    EXPECT_EQ(orc.get_position().get_y(), 7);
    
    auto result = orc.vs(druid);
    EXPECT_TRUE(result.has_value());
}
