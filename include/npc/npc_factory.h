#pragma once

#include "npc.h"
#include "../geometry/point.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>

class NPCFactory {
public:
    NPCFactory();
    ~NPCFactory() = default;
    
    // Запрет копирования
    NPCFactory(const NPCFactory&) = delete;
    NPCFactory& operator=(const NPCFactory&) = delete;
    
    // Command: создание NPC по типу
    std::unique_ptr<NPC> create(const std::string& type, const std::string& name, const Point& position) const;
    
    // Command: загрузка NPC из файла
    std::vector<std::unique_ptr<NPC>> load_from_file(const std::string& filename) const;
    
    // Command: сохранение NPC в файл
    void save_to_file(const std::string& filename, const std::vector<std::unique_ptr<NPC>>& npcs) const;

private:
    // Внутренняя структура для регистрации создателей
    using CreatorFunc = std::function<std::unique_ptr<NPC>(const std::string&, const Point&)>;
    std::vector<std::pair<std::string, CreatorFunc>> creators;
    
    void register_creators();
};

