#include "../../include/npc/orc.h"
#include "../../include/battle/visitor.h"

Orc::Orc(const std::string& name, const Point& position) : NPC(name, position) {}

std::string Orc::get_type() const { 
    return "Орк"; 
}

void Orc::accept(Visitor& visitor) { 
    visitor.visit(*this); 
}

std::optional<std::string> Orc::vs(const NPC& target) const {
    if (!this->is_alive() || !target.is_alive()) {
        return std::nullopt; 
    }
    const std::string& type = target.get_type();

    // Орки убивают друидов
    if (type == "Друид") {
        return "Орк разорвал бедолагу Друида!";
    }

    return std::nullopt;
}

int Orc::get_move_distance() const {
    return 20; // Орк: расстояние хода 20
}

int Orc::get_kill_distance() const {
    return 10; // Орк: расстояние убийства 10
}

