#include "../../include/npc/druid.h"
#include "../../include/battle/visitor.h"

Druid::Druid(const std::string& name, const Point& position) : NPC(name, position) {}

std::string Druid::get_type() const { 
    return "Друид"; 
}

void Druid::accept(Visitor& visitor) { 
    visitor.visit(*this); 
}

std::optional<std::string> Druid::vs(const NPC& target) const {
    if (!this->is_alive() || !target.is_alive()) {
        return std::nullopt;
    }
    const std::string& type = target.get_type();

    // Друиды убивают Белок
    if (type == "Белка") {
        return "Друид уничтожил Белку!";
    }

    return std::nullopt;
}

