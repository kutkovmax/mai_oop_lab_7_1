#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <queue>
#include <random>
#include "../npc/npc.h"
#include "../npc/npc_factory.h"

// Структура для задачи боя
struct BattleTask {
    NPC* attacker;
    NPC* target;
    
    BattleTask(NPC* a, NPC* t) : attacker(a), target(t) {}
};

// Класс для управления игрой с потоками
class Game {
public:
    static constexpr int MAP_WIDTH = 50;
    static constexpr int MAP_HEIGHT = 50;
    static constexpr int GAME_DURATION_SECONDS = 30;
    static constexpr int NUM_NPCS = 50;
    
    Game();
    ~Game();
    
    // Запрет копирования
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    
    // Запуск игры
    void start();
    
    // Остановка игры
    void stop();
    
    // Получить список выживших NPC
    std::vector<std::string> get_survivors() const;

private:
    std::vector<std::unique_ptr<NPC>> npcs;
    std::unique_ptr<NPCFactory> factory;
    
    // Потоки
    std::thread movement_thread;
    std::thread battle_thread;
    std::thread main_thread;
    
    // Синхронизация
    mutable std::shared_mutex npcs_mutex; // Для чтения/записи NPC
    mutable std::mutex cout_mutex; // Для защиты std::cout
    std::mutex battle_queue_mutex; // Для очереди боев
    
    // Очередь задач боев
    std::queue<BattleTask> battle_queue;
    
    // Флаги управления
    std::atomic<bool> running;
    std::atomic<bool> game_over;
    
    // Генераторы случайных чисел (по одному на поток для thread-safety)
    mutable std::mt19937 movement_rng;
    mutable std::mt19937 battle_rng;
    mutable std::mt19937 init_rng;
    
    // Приватные методы потоков
    void movement_worker();
    void battle_worker();
    void main_worker();
    
    // Вспомогательные методы
    void initialize_npcs();
    void print_map() const;
    int roll_dice() const; // Бросок 6-гранного кубика
    void process_battle(NPC* attacker, NPC* target);
    Point random_position() const;
};

