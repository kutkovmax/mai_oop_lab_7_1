#include "../../include/npc/squirrel.h"
#include "../../include/battle/visitor.h"

Squirrel::Squirrel(const std::string& name, const Point& position) : NPC(name, position) {}

std::string Squirrel::get_type() const { 
    return "Белка"; 
}

void Squirrel::accept(Visitor& visitor) { 
    visitor.visit(*this); 
}

// Белки за мир - они никого не убивают
std::optional<std::string> Squirrel::vs(const NPC& target) const {
    return std::nullopt;
}

int Squirrel::get_move_distance() const {
    return 5; // Белка: расстояние хода 5
}

int Squirrel::get_kill_distance() const {
    return 5; // Белка: расстояние убийства 5
}

