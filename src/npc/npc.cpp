#include "../../include/npc/npc.h"

NPC::NPC(const std::string& name, const Point& position) 
    : name(name), position(position), alive(true) {}

std::string NPC::get_name() const { 
    return name; 
}

Point NPC::get_position() const { 
    return position; 
}

bool NPC::is_alive() const { 
    return alive; 
}

void NPC::kill() { 
    alive = false; 
}
