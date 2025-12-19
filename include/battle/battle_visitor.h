#pragma once

#include "visitor.h"
#include "event_manager.h"
#include "observer.h"
#include "../npc/npc.h"
#include <memory>

// Forward declarations
class Druid;
class Orc;
class Squirrel;

class BattleVisitor : public Visitor {
public:
    explicit BattleVisitor(double radius);
    ~BattleVisitor() = default;
    
    // Запрет копирования (Rule of Five)
    BattleVisitor(const BattleVisitor&) = delete;
    BattleVisitor& operator=(const BattleVisitor&) = delete;
    
    // Command: установка атакующего
    void set_attacker(NPC* attacker);
    
    // Query: получение радиуса (не изменяет состояние)
    double get_radius() const;
    
    // Command: добавление наблюдателя
    void subscribe(Observer* observer);
    
    // Command: выполнение боя
    void perform_battle(NPC& target);
    
    // Visitor pattern методы
    void visit(Druid& druid) override;
    void visit(Orc& orc) override;
    void visit(Squirrel& squirrel) override;

private:
    NPC* current_attacker;
    double battle_radius;
    EventManager event_manager;
    
    // Приватный метод для логики боя (Tell Don't Ask)
    void execute_battle_logic(NPC& attacker, NPC& target);
    void notify_kill(const std::string& action, const std::string& killer_name, const std::string& victim_name);
};

