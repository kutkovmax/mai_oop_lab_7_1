#include "../../include/game/game.h"
#include "../../include/npc/orc.h"
#include "../../include/npc/squirrel.h"
#include "../../include/npc/druid.h"
#include "../../include/geometry/point.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <algorithm>
#include <optional>
#include <cmath>

Game::Game() 
    : factory(std::make_unique<NPCFactory>()),
      running(false),
      game_over(false),
      movement_rng(std::random_device{}()),
      battle_rng(std::random_device{}()),
      init_rng(std::random_device{}()) {
    initialize_npcs();
}

Game::~Game() {
    stop();
}

void Game::initialize_npcs() {
    std::vector<std::string> types = {"Орк", "Белка", "Друид"};
    std::uniform_int_distribution<int> type_dist(0, types.size() - 1);
    std::uniform_int_distribution<int> name_dist(1, 9999);
    
    for (int i = 0; i < NUM_NPCS; ++i) {
        std::string type = types[type_dist(init_rng)];
        std::string name = type + "_" + std::to_string(name_dist(init_rng));
        Point pos = random_position();
        
        auto npc = factory->create(type, name, pos);
        npcs.push_back(std::move(npc));
    }
}

Point Game::random_position() const {
    std::uniform_int_distribution<int> x_dist(0, MAP_WIDTH - 1);
    std::uniform_int_distribution<int> y_dist(0, MAP_HEIGHT - 1);
    return Point(x_dist(init_rng), y_dist(init_rng));
}

void Game::start() {
    running = true;
    game_over = false;
    
    // Запускаем потоки
    movement_thread = std::thread(&Game::movement_worker, this);
    battle_thread = std::thread(&Game::battle_worker, this);
    main_thread = std::thread(&Game::main_worker, this);
    
    // Ждем завершения основного потока (30 секунд)
    if (main_thread.joinable()) {
        main_thread.join();
    }
    
    stop();
}

void Game::stop() {
    running = false;
    
    if (movement_thread.joinable()) {
        movement_thread.join();
    }
    if (battle_thread.joinable()) {
        battle_thread.join();
    }
}

void Game::movement_worker() {
    std::uniform_real_distribution<double> angle_dist(0.0, 2.0 * M_PI);

    while (running) {
        // === 1. ДВИЖЕНИЕ NPC ===
        {
            std::unique_lock<std::shared_mutex> lock(npcs_mutex);

            for (auto& npc_ptr : npcs) {
                NPC* npc = npc_ptr.get();
                if (!npc || !npc->is_alive()) continue; // аааа некроманты

                double angle = angle_dist(movement_rng);
                int move_dist = npc->get_move_distance();

                int dx = static_cast<int>(std::round(std::cos(angle) * move_dist));
                int dy = static_cast<int>(std::round(std::sin(angle) * move_dist));

                npc->move(dx, dy, MAP_WIDTH, MAP_HEIGHT);
            }
        }

        // === 2. ПОИСК БОЁВ ===
        {
            std::shared_lock<std::shared_mutex> read_lock(npcs_mutex);

            for (size_t i = 0; i < npcs.size(); ++i) {
                NPC* a = npcs[i].get();
                if (!a || !a->is_alive()) continue;

                for (size_t j = i + 1; j < npcs.size(); ++j) {
                    NPC* b = npcs[j].get();
                    if (!b || !b->is_alive()) continue;

                    Point pa = a->get_position();
                    Point pb = b->get_position();

                    double dx = pa.get_x() - pb.get_x();
                    double dy = pa.get_y() - pb.get_y();
                    double dist = std::sqrt(dx * dx + dy * dy);

                    // a -> b
                    if (dist <= a->get_kill_distance()) {
                        std::lock_guard<std::mutex> ql(battle_queue_mutex);
                        battle_queue.push({a, b});
                    }

                    // b -> a
                    if (dist <= b->get_kill_distance()) {
                        std::lock_guard<std::mutex> ql(battle_queue_mutex);
                        battle_queue.push({b, a});
                    }
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    {
        std::lock_guard<std::mutex> lg(cout_mutex);
        std::cout << "[MOVE] movement_worker STOPPED\n";
    }
}

void Game::battle_worker() {
    while (running || !battle_queue.empty()) {
        std::optional<BattleTask> task;
        
        // Получаем задачу из очереди
        {
            std::lock_guard<std::mutex> queue_lock(battle_queue_mutex);
            if (!battle_queue.empty()) {
                task = battle_queue.front();
                battle_queue.pop();
            }
        }
        
        if (task.has_value()) {
            // Проверяем, что оба NPC еще живы
            {
                if (task->attacker->is_alive() && task->target->is_alive()) {
                    process_battle(task->attacker, task->target);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void Game::process_battle(NPC* attacker, NPC* target) {
    // Проверяем, может ли attacker убить target
    auto kill_result = attacker->vs(*target);
    if (!kill_result.has_value()) {
        return; // Не может убить
    }
    
    // Каждый NPC "кидает 6-гранный кубик" для атаки и защиты
    int attack_power = roll_dice();
    int defense_power = roll_dice();
    
    // Если сила атаки больше силы защиты - происходит убийство
    if (attack_power > defense_power) {
        std::unique_lock<std::shared_mutex> write_lock(npcs_mutex);
        if (target->is_alive()) { // Проверяем еще раз
            target->kill();
            
            // Выводим информацию о бое
            {
                std::lock_guard<std::mutex> cout_lock(cout_mutex);
                std::cout << kill_result.value() 
                          << " [Атака: " << attack_power 
                          << " > Защита: " << defense_power << "]\n";
            }
        }
    } else {
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << attacker->get_name() << " атаковал " << target->get_name()
                  << " но защита была сильнее! [Атака: " << attack_power 
                  << " <= Защита: " << defense_power << "]\n";
    }
}

int Game::roll_dice() const {
    std::uniform_int_distribution<int> dice(1, 6);
    return dice(battle_rng);
}

void Game::main_worker() {
    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(GAME_DURATION_SECONDS);
    
    while (running && std::chrono::steady_clock::now() < end_time) {
        print_map();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    game_over = true;
    
    // Финальный вывод карты и списка выживших
    print_map();
    
    {
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << "\n=== ИГРА ЗАВЕРШЕНА ===\n";
        std::cout << "Выжившие NPC:\n";
        
        auto survivors = get_survivors();
        for (const auto& name : survivors) {
            std::cout << "  - " << name << "\n";
        }
        std::cout << "Всего выжило: " << survivors.size() << "\n";
    }
}

void Game::print_map() const {
    std::lock_guard<std::mutex> cout_lock(cout_mutex);
    
    // std::cout << "\033[2J\033[H";
    std::cout << "=== КАРТА ПОДЗЕМЕЛЬЯ ===\n";
    
    // Подсчитываем живых NPC
    size_t alive_count = 0;
    {
        std::shared_lock<std::shared_mutex> read_lock(npcs_mutex);
        alive_count = std::count_if(npcs.begin(), npcs.end(),
            [](const auto& npc) { return npc && npc->is_alive(); });
    }
    
    std::cout << "Живых NPC: " << alive_count << "\n\n";
    
    // Создаем карту
    std::vector<std::vector<char>> map(MAP_HEIGHT, std::vector<char>(MAP_WIDTH, '.'));
    
    {
        std::shared_lock<std::shared_mutex> read_lock(npcs_mutex);
        for (const auto& npc : npcs) {
            if (npc && npc->is_alive()) {
                Point pos = npc->get_position();
                if (pos.get_x() >= 0 && pos.get_x() < MAP_WIDTH &&
                    pos.get_y() >= 0 && pos.get_y() < MAP_HEIGHT) {
                    char symbol = '?';
                    std::string type = npc->get_type();
                    if (type == "Орк") symbol = 'O';
                    else if (type == "Белка") symbol = 'S';
                    else if (type == "Друид") symbol = 'D';
                    
                    map[pos.get_y()][pos.get_x()] = symbol;
                }
            }
        }
    }
    
    // Выводим карту (полная версия для лучшей видимости движения)
    // Показываем каждую клетку карты напрямую
    const int DISPLAY_WIDTH = 50;
    const int DISPLAY_HEIGHT = 50; // Показываем половину высоты для читаемости
    
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            if (y < MAP_HEIGHT && x < MAP_WIDTH) {
                std::cout << map[y][x];
            } else {
                std::cout << ' ';
            }
        }
        std::cout << "\n";
    }
    // std::cout.flush();
}

std::vector<std::string> Game::get_survivors() const {
    std::vector<std::string> survivors;
    
    std::shared_lock<std::shared_mutex> read_lock(npcs_mutex);
    for (const auto& npc : npcs) {
        if (npc && npc->is_alive()) {
            survivors.push_back(npc->get_name() + " (" + npc->get_type() + ")");
        }
    }
    
    return survivors;
}

