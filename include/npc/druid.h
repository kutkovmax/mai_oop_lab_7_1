#pragma once

#include "npc.h"

class Druid : public NPC {
public:
    Druid(const std::string& name, const Point& position);

    std::string get_type() const override;
    void accept(Visitor& visitor) override;
    std::optional<std::string> vs(const NPC& target) const override;
    int get_move_distance() const override;
    int get_kill_distance() const override;
};

