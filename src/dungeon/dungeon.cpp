#include "../../include/dungeon/dungeon.h"
#include "../../include/npc/npc.h"
#include "../../include/npc/npc_factory.h"
#include "../../include/battle/battle_visitor.h"
#include "../../include/battle/console_observer.h"
#include "../../include/battle/file_observer.h"
#include "../../include/geometry/point.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

DungeonEditor::DungeonEditor() 
    : factory(std::make_unique<NPCFactory>()) {
    initialize_observers();
}

DungeonEditor::~DungeonEditor() = default;

void DungeonEditor::initialize_observers() {
    console_observer = std::make_unique<ConsoleObserver>();
    file_observer = std::make_unique<FileObserver>("log.txt");
}

void DungeonEditor::add_npc(const std::string& type, const std::string& name, int x, int y) {
    try {
        if (is_name_exists(name)) {
            return;
        }
        Point position(x, y);
        auto npc = factory->create(type, name, position);
        npcs.push_back(std::move(npc));
        std::cout << "Добавлен " << type << " '" << name << "' в позиции (" << x << ", " << y << ")\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при создании NPC: " << e.what() << std::endl;
    }
}

void DungeonEditor::save_to_file(const std::string& filename) {
    try {
        factory->save_to_file(filename, npcs);
        std::cout << "Данные сохранены в файл: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при сохранении: " << e.what() << std::endl;
    }
}

void DungeonEditor::load_from_file(const std::string& filename) {
    try {
        auto loaded_npcs = factory->load_from_file(filename);
        npcs = std::move(loaded_npcs);
        std::cout << "Данные загружены из файла: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке: " << e.what() << std::endl;
    }
}

void DungeonEditor::print_all() const {
    std::cout << "\n=== СПИСОК NPC ===\n";
    for (const auto& npc : npcs) {
        if (npc && npc->is_alive()) {
            auto pos = npc->get_position();
            std::cout << "Имя: " << npc->get_name()
                      << " | Тип: " << npc->get_type()
                      << " | Позиция: (" << pos.get_x() << ", " << pos.get_y() << ")\n";
        }
    }
    std::cout << "Всего живых NPC: " << get_alive_count() << "\n\n";
}

void DungeonEditor::start_battle(double radius) {
    std::cout << "=== НАЧАЛО БИТВЫ (радиус: " << radius << ") ===\n";
    
    BattleVisitor battle_visitor(radius);
    battle_visitor.subscribe(console_observer.get());
    battle_visitor.subscribe(file_observer.get());
    
    // Tell Don't Ask: говорим visitor'у выполнить битву, не спрашиваем детали
    for (size_t i = 0; i < npcs.size(); ++i) {
        if (!npcs[i] || !npcs[i]->is_alive()) continue;
        
        for (size_t j = i + 1; j < npcs.size(); ++j) {
            if (!npcs[j] || !npcs[j]->is_alive()) continue;
            
            double distance = npcs[i]->get_position().distance_to(npcs[j]->get_position());
            
            if (distance <= radius) {
                battle_visitor.set_attacker(npcs[i].get());
                npcs[j]->accept(battle_visitor);
            }
        }
    }
    
    cleanup_dead_npcs();
    
    std::cout << "=== БИТВА ЗАВЕРШЕНА ===\n";
    std::cout << "Осталось живых NPC: " << get_alive_count() << "\n\n";
}

size_t DungeonEditor::get_alive_count() const {
    return std::count_if(npcs.begin(), npcs.end(),
        [](const auto& npc) {
            return npc != nullptr && npc->is_alive();
        });
}

bool DungeonEditor::is_name_exists(const std::string& name) const {
    return std::any_of(npcs.begin(), npcs.end(),
        [&name](const auto& npc) {
            return npc && npc->get_name() == name;
        });
}

void DungeonEditor::remove_dead_npcs() {
    cleanup_dead_npcs();
}

void DungeonEditor::cleanup_dead_npcs() {
    npcs.erase(
        std::remove_if(npcs.begin(), npcs.end(),
            [](const auto& npc) {
                return !npc || !npc->is_alive();
            }),
        npcs.end()
    );
}
