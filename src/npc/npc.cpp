#include "../../include/npc/npc.h"
#include <algorithm>

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

void NPC::move(int dx, int dy, int map_width, int map_height) {
    if (!alive) return; // Мертвые не передвигаются
    
    int new_x = position.get_x() + dx;
    int new_y = position.get_y() + dy;
    
    // Живые NPC не могут покинуть карту
    new_x = std::max(0, std::min(new_x, map_width - 1));
    new_y = std::max(0, std::min(new_y, map_height - 1));
    
    position.set_x(new_x);
    position.set_y(new_y);
}
