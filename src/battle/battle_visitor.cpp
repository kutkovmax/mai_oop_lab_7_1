#include "../../include/battle/battle_visitor.h"
#include "../../include/battle/battle_event.h"
#include "../../include/npc/orc.h"
#include "../../include/npc/druid.h"
#include "../../include/npc/squirrel.h"
#include <algorithm>

BattleVisitor::BattleVisitor(double radius) 
    : current_attacker(nullptr), battle_radius(radius) {}

void BattleVisitor::set_attacker(NPC* attacker) {
    current_attacker = attacker;
}

double BattleVisitor::get_radius() const {
    return battle_radius;
}

void BattleVisitor::subscribe(Observer* observer) {
    event_manager.subscribe(observer);
}

void BattleVisitor::perform_battle(NPC& target) {
    if (!current_attacker || !current_attacker->is_alive() || !target.is_alive()) {
        return;
    }

    double distance = current_attacker->get_position().distance_to(target.get_position());
    
    if (distance > battle_radius) {
        return;
    }

    execute_battle_logic(*current_attacker, target);
}

void BattleVisitor::execute_battle_logic(NPC& attacker, NPC& target) {
    // Проверяем обе стороны на возможность убийства (взаимные убийства)
    auto action1 = attacker.vs(target);
    auto action2 = target.vs(attacker);

    // Tell Don't Ask: говорим цели умереть, если атакующий может убить
    if (action1.has_value()) {
        target.kill();
        notify_kill(action1.value(), attacker.get_name(), target.get_name());
    }

    // Tell Don't Ask: говорим атакующему умереть, если цель может убить
    if (action2.has_value()) {
        attacker.kill();
        notify_kill(action2.value(), target.get_name(), attacker.get_name());
    }
}

void BattleVisitor::notify_kill(const std::string& action, const std::string& killer_name, const std::string& victim_name) {
    BattleEvent event;
    event.action = action + " (" + killer_name + " убивает " + victim_name + ")";
    event_manager.publish(event);
}

void BattleVisitor::visit(Druid& druid) {
    perform_battle(druid);
}

void BattleVisitor::visit(Orc& orc) {
    perform_battle(orc);
}

void BattleVisitor::visit(Squirrel& squirrel) {
    perform_battle(squirrel);
}

