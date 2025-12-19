#include "../../include/npc/npc_factory.h"
#include "../../include/npc/orc.h"
#include "../../include/npc/druid.h"
#include "../../include/npc/squirrel.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

NPCFactory::NPCFactory() {
    register_creators();
}

void NPCFactory::register_creators() {
    creators.emplace_back("Орк", [](const std::string& name, const Point& pos) {
        return std::make_unique<Orc>(name, pos);
    });
    
    creators.emplace_back("Друид", [](const std::string& name, const Point& pos) {
        return std::make_unique<Druid>(name, pos);
    });
    
    creators.emplace_back("Белка", [](const std::string& name, const Point& pos) {
        return std::make_unique<Squirrel>(name, pos);
    });
}

std::unique_ptr<NPC> NPCFactory::create(const std::string& type, 
                                        const std::string& name, 
                                        const Point& position) const {
    for (const auto& [creator_type, creator_func] : creators) {
        if (creator_type == type) {
            return creator_func(name, position);
        }
    }
    throw std::invalid_argument("Неизвестный тип NPC: " + type);
}

std::vector<std::unique_ptr<NPC>> NPCFactory::load_from_file(const std::string& filename) const {
    std::vector<std::unique_ptr<NPC>> npcs;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);       
        std::string name, type;
        int x, y;

        if (iss >> name >> type >> x >> y) {
            try {
                Point position(x, y);
                auto npc = create(type, name, position);
                npcs.push_back(std::move(npc));
            } catch (const std::exception& e) {
                std::cerr << "Ошибка создания NPC: " << e.what() << std::endl;
            }
        }
    }

    return npcs;
}

void NPCFactory::save_to_file(const std::string& filename, 
                              const std::vector<std::unique_ptr<NPC>>& npcs) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для записи: " + filename);
    }

    for (const auto& npc : npcs) {
        if (npc && npc->is_alive()) { 
            file << npc->get_name() << " " 
                 << npc->get_type() << " "  
                 << npc->get_position().get_x() << " " 
                 << npc->get_position().get_y() << "\n"; 
        }
    }
}

