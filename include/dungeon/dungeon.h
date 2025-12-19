#pragma once

#include <vector>
#include <memory>
#include <string>

// Forward declarations для уменьшения связности
class NPC;
class NPCFactory;
class BattleVisitor;
class ConsoleObserver;
class FileObserver;

// Single Responsibility: управление подземельем и NPC
// High Cohesion: все методы связаны с управлением подземельем
class DungeonEditor {
public:
    DungeonEditor();
    ~DungeonEditor();
    
    // Запрет копирования
    DungeonEditor(const DungeonEditor&) = delete;
    DungeonEditor& operator=(const DungeonEditor&) = delete;
    
    // Command: добавление NPC (изменяет состояние)
    void add_npc(const std::string& type, const std::string& name, int x, int y);
    
    // Command: сохранение в файл
    void save_to_file(const std::string& filename);
    
    // Command: загрузка из файла
    void load_from_file(const std::string& filename);
    
    // Query: печать списка (не изменяет состояние)
    void print_all() const;
    
    // Command: запуск боя (изменяет состояние)
    void start_battle(double radius);
    
    // Query: количество живых NPC
    size_t get_alive_count() const;
    
    // Query: проверка существования имени
    bool is_name_exists(const std::string& name) const;
    
    // Command: удаление мертвых NPC
    void remove_dead_npcs();

private:
    std::vector<std::unique_ptr<NPC>> npcs;
    std::unique_ptr<NPCFactory> factory;
    std::unique_ptr<ConsoleObserver> console_observer;
    std::unique_ptr<FileObserver> file_observer;
    
    // Приватные вспомогательные методы (Tell Don't Ask)
    void initialize_observers();
    void cleanup_dead_npcs();
};
