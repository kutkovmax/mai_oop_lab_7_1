#include "../include/battle/battle_visitor.h"
#include "../include/battle/console_observer.h"
#include "../include/battle/file_observer.h"
#include "../include/npc/druid.h"
#include "../include/npc/orc.h"
#include "../include/npc/squirrel.h"
#include "../include/geometry/point.h"
#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include <sstream>


class BattleVisitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        visitor = std::make_unique<BattleVisitor>(10.0);
    }
    
    std::unique_ptr<BattleVisitor> visitor;
};

TEST_F(BattleVisitorTest, ConstructorAndGetters) {
    EXPECT_EQ(visitor->get_radius(), 10.0);
    
    BattleVisitor visitor2(5.5);
    EXPECT_EQ(visitor2.get_radius(), 5.5);
}

TEST_F(BattleVisitorTest, SetAttacker) {
    Orc orc("Орк", Point(0, 0));
    
    EXPECT_NO_THROW({
        visitor->set_attacker(&orc);
    });
}

TEST_F(BattleVisitorTest, SubscribeObserver) {
    ConsoleObserver console_observer;
    
    EXPECT_NO_THROW({
        visitor->subscribe(&console_observer);
    });
}

TEST_F(BattleVisitorTest, BattleOutOfRange) {
    Orc attacker("Атакующий", Point(0, 0));
    Druid target("Цель", Point(100, 100)); // Далеко за пределами радиуса
    
    visitor->set_attacker(&attacker);
    
    // Цель должна остаться живой (слишком далеко)
    EXPECT_TRUE(target.is_alive());
    visitor->visit(target);
    EXPECT_TRUE(target.is_alive()); // Все еще жива
}

TEST_F(BattleVisitorTest, BattleWithinRange) {
    Orc attacker("Атакующий", Point(0, 0));
    Druid target("Цель", Point(1, 1)); // В пределах радиуса
    
    visitor->set_attacker(&attacker);
    
    // Добавляем наблюдателя для перехвата вывода
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    visitor->subscribe(&console_observer);
    
    visitor->visit(target);
    
    std::string output = testing::internal::GetCapturedStdout();
    
    // Орк должен убить друида
    EXPECT_FALSE(target.is_alive());
    EXPECT_TRUE(output.find("Орк разорвал бедолагу Друида") != std::string::npos);
}

TEST_F(BattleVisitorTest, BattleDeadAttacker) {
    Orc attacker("Атакующий", Point(0, 0));
    Druid target("Цель", Point(1, 1));
    
    // Убиваем атакующего
    attacker.kill();
    visitor->set_attacker(&attacker);
    
    visitor->visit(target);
    
    // Цель должна остаться живой (атакующий мертв)
    EXPECT_TRUE(target.is_alive());
}

TEST_F(BattleVisitorTest, BattleDeadTarget) {
    Orc attacker("Атакующий", Point(0, 0));
    Druid target("Цель", Point(1, 1));
    
    // Убиваем цель
    target.kill();
    visitor->set_attacker(&attacker);
    
    visitor->visit(target);
    
    // Цель уже мертва, ничего не должно произойти
    EXPECT_FALSE(target.is_alive());
}

TEST_F(BattleVisitorTest, BattleNoAttacker) {
    Druid target("Цель", Point(1, 1));
    
    // Не устанавливаем атакующего
    visitor->visit(target);
    
    // Цель должна остаться живой (нет атакующего)
    EXPECT_TRUE(target.is_alive());
}

TEST_F(BattleVisitorTest, VisitDifferentTypes) {
    Orc attacker("Орк", Point(0, 0));
    visitor->set_attacker(&attacker);
    
    // Тестируем посещение разных типов NPC
    Druid druid("Друид", Point(1, 1));
    Orc orc("Орк2", Point(2, 2));
    Squirrel squirrel("Белка", Point(3, 3));
    
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    visitor->subscribe(&console_observer);
    
    // Все должны быть атакованы (в пределах радиуса)
    visitor->visit(druid);
    visitor->visit(orc);
    visitor->visit(squirrel);
    
    std::string output = testing::internal::GetCapturedStdout();
    
    // Проверяем что были соответствующие действия (только Орк убивает Друида)
    EXPECT_TRUE(output.find("Друида") != std::string::npos);
}

TEST_F(BattleVisitorTest, BattleWithFileObserver) {
    const std::string test_file = "battle_test_log.txt";
    
    Orc attacker("Орк", Point(0, 0));
    Druid target("Друид", Point(1, 1));
    
    visitor->set_attacker(&attacker);
    
    // Добавляем файлового наблюдателя
    FileObserver file_observer(test_file);
    visitor->subscribe(&file_observer);
    
    // Удаляем старый файл если существует
    std::remove(test_file.c_str());
    
    visitor->visit(target);
    
    // Проверяем что файл создан и содержит запись о битве
    std::ifstream file(test_file);
    ASSERT_TRUE(file.is_open());
    
    std::string content;
    std::getline(file, content);
    EXPECT_TRUE(content.find("Орк разорвал бедолагу Друида") != std::string::npos);
    EXPECT_TRUE(content.find("[File] Action:") != std::string::npos);
    
    file.close();
    std::remove(test_file.c_str());
}

TEST_F(BattleVisitorTest, BattleInteractionOrcVsDruid) {
    Orc orc("Орк", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    
    visitor->set_attacker(&orc);
    
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    visitor->subscribe(&console_observer);
    
    visitor->visit(druid);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_FALSE(druid.is_alive());
    EXPECT_TRUE(output.find("Орк разорвал бедолагу Друида") != std::string::npos);
}

TEST_F(BattleVisitorTest, BattleInteractionDruidVsSquirrel) {
    Druid druid("Друид", Point(0, 0));
    Squirrel squirrel("Белка", Point(1, 1));
    
    visitor->set_attacker(&druid);
    
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    visitor->subscribe(&console_observer);
    
    visitor->visit(squirrel);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_FALSE(squirrel.is_alive());
    EXPECT_TRUE(output.find("Друид уничтожил Белку") != std::string::npos);
}

TEST_F(BattleVisitorTest, BattleInteractionSquirrelNeverAttacks) {
    Squirrel squirrel("Белка", Point(0, 0));
    Druid druid("Друид", Point(1, 1));
    
    visitor->set_attacker(&squirrel);
    
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    visitor->subscribe(&console_observer);
    
    visitor->visit(druid);
    std::string output = testing::internal::GetCapturedStdout();
    
    // Белка не должна атаковать, но Друид может атаковать Белку (но не здесь, т.к. атакует Белка)
    EXPECT_TRUE(druid.is_alive());
    // Но Друид может убить Белку если атакует обратно - но это проверяется в другом тесте
}

TEST_F(BattleVisitorTest, BattleInteractionOrcVsSquirrel) {
    Orc orc("Орк", Point(0, 0));
    Squirrel squirrel("Белка", Point(1, 1));
    
    visitor->set_attacker(&orc);
    
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    visitor->subscribe(&console_observer);
    
    visitor->visit(squirrel);
    std::string output = testing::internal::GetCapturedStdout();
    
    // Орк не убивает Белку, Белка не убивает Орка - оба должны остаться живы
    EXPECT_TRUE(squirrel.is_alive());
    EXPECT_TRUE(output.empty() || output.find("Белку") == std::string::npos);
}

TEST_F(BattleVisitorTest, BattleZeroRadius) {
    BattleVisitor zero_radius_visitor(0.0);
    Orc attacker("Орк", Point(0, 0));
    Druid target("Друид", Point(0, 0)); // Та же позиция
    
    zero_radius_visitor.set_attacker(&attacker);
    
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    zero_radius_visitor.subscribe(&console_observer);
    
    zero_radius_visitor.visit(target);
    std::string output = testing::internal::GetCapturedStdout();
    
    // При нулевом радиусе и одинаковой позиции расстояние = 0, битва должна произойти
    EXPECT_FALSE(target.is_alive());
    EXPECT_FALSE(output.empty());
}

TEST_F(BattleVisitorTest, BattleExactRadius) {
    // Расстояние между (0,0) и (10,0) = 10.0, что равно радиусу
    BattleVisitor exact_radius_visitor(10.0);
    Orc attacker("Орк", Point(0, 0));
    Druid target("Друид", Point(10, 0));
    
    exact_radius_visitor.set_attacker(&attacker);
    
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    exact_radius_visitor.subscribe(&console_observer);
    
    exact_radius_visitor.visit(target);
    std::string output = testing::internal::GetCapturedStdout();
    
    // Битва должна произойти (расстояние равно радиусу)
    EXPECT_FALSE(target.is_alive());
    EXPECT_FALSE(output.empty());
}

// Тесты для проверки неизменяемости состояния
TEST_F(BattleVisitorTest, StateImmutability) {
    Orc attacker("Орк", Point(0, 0));
    Druid target("Друид", Point(1, 1));
    
    // Сохраняем исходное состояние
    Point original_attacker_pos = attacker.get_position();
    Point original_target_pos = target.get_position();
    std::string original_attacker_name = attacker.get_name();
    std::string original_target_name = target.get_name();
    
    visitor->set_attacker(&attacker);
    
    testing::internal::CaptureStdout();
    ConsoleObserver console_observer;
    visitor->subscribe(&console_observer);
    
    visitor->visit(target);
    std::string output = testing::internal::GetCapturedStdout();
    
    // Проверяем что позиции и имена не изменились (кроме состояния alive)
    EXPECT_EQ(attacker.get_position().get_x(), original_attacker_pos.get_x());
    EXPECT_EQ(attacker.get_position().get_y(), original_attacker_pos.get_y());
    EXPECT_EQ(target.get_position().get_x(), original_target_pos.get_x());
    EXPECT_EQ(target.get_position().get_y(), original_target_pos.get_y());
    EXPECT_EQ(attacker.get_name(), original_attacker_name);
    EXPECT_EQ(target.get_name(), original_target_name);
}
