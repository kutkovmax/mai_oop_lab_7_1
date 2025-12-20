#include <gtest/gtest.h>
#include "../include/game/game.h"
#include "../include/npc/orc.h"
#include "../include/npc/squirrel.h"
#include "../include/npc/druid.h"
#include "../include/geometry/point.h"
#include <thread>
#include <chrono>

// Тест проверки расстояний хода и убийства
TEST(GameTest, NPCMoveAndKillDistances) {
    Point pos(50, 50);
    
    Orc orc("TestOrc", pos);
    EXPECT_EQ(orc.get_move_distance(), 20);
    EXPECT_EQ(orc.get_kill_distance(), 10);
    
    Squirrel squirrel("TestSquirrel", pos);
    EXPECT_EQ(squirrel.get_move_distance(), 5);
    EXPECT_EQ(squirrel.get_kill_distance(), 5);
    
    Druid druid("TestDruid", pos);
    EXPECT_EQ(druid.get_move_distance(), 10);
    EXPECT_EQ(druid.get_kill_distance(), 10);
}

// Тест проверки границ карты
TEST(GameTest, MapBoundaries) {
    Point pos1(0, 0);
    Point pos2(99, 99);
    Point pos3(100, 100); // За границей
    
    Orc orc("TestOrc", pos1);
    
    // Движение в пределах карты
    orc.move(10, 10, 100, 100);
    Point new_pos = orc.get_position();
    EXPECT_GE(new_pos.get_x(), 0);
    EXPECT_GE(new_pos.get_y(), 0);
    EXPECT_LT(new_pos.get_x(), 100);
    EXPECT_LT(new_pos.get_y(), 100);
    
    // Попытка выйти за границы
    orc.move(200, 200, 100, 100);
    new_pos = orc.get_position();
    EXPECT_LT(new_pos.get_x(), 100);
    EXPECT_LT(new_pos.get_y(), 100);
}

// Тест проверки мертвых NPC не двигаются
TEST(GameTest, DeadNPCsDontMove) {
    Point pos(50, 50);
    Orc orc("TestOrc", pos);
    
    Point original_pos = orc.get_position();
    orc.kill();
    
    // Мертвый NPC не должен двигаться
    orc.move(10, 10, 100, 100);
    Point new_pos = orc.get_position();
    
    EXPECT_EQ(original_pos.get_x(), new_pos.get_x());
    EXPECT_EQ(original_pos.get_y(), new_pos.get_y());
}

// Тест проверки создания NPC
TEST(GameTest, NPCInitialization) {
    Game game;
    
    // Проверяем, что создалось правильное количество NPC
    auto survivors = game.get_survivors();
    EXPECT_EQ(survivors.size(), Game::NUM_NPCS);
}

// Тест проверки координат дискретные
TEST(GameTest, DiscreteCoordinates) {
    Point pos(50, 50);
    
    // Проверяем, что координаты int
    int x = pos.get_x();
    int y = pos.get_y();
    
    EXPECT_EQ(x, 50);
    EXPECT_EQ(y, 50);
    
    // Проверяем, что можно установить только целые значения
    pos.set_x(75);
    pos.set_y(25);
    EXPECT_EQ(pos.get_x(), 75);
    EXPECT_EQ(pos.get_y(), 25);
}

// Тест проверки расстояния (double для вычисления, но сравнение с int)
TEST(GameTest, DistanceCalculation) {
    Point pos1(0, 0);
    Point pos2(3, 4);
    
    double distance = pos1.distance_to(pos2);
    // Расстояние должно быть 5.0 (3-4-5 треугольник)
    EXPECT_NEAR(distance, 5.0, 0.001);
    
    // Но координаты остаются int
    EXPECT_EQ(pos1.get_x(), 0);
    EXPECT_EQ(pos1.get_y(), 0);
    EXPECT_EQ(pos2.get_x(), 3);
    EXPECT_EQ(pos2.get_y(), 4);
}

