#pragma once

#include "npc.h"

class Orc : public NPC {
public:
    Orc(const std::string& name, const Point& position);

    std::string get_type() const override;
    void accept(Visitor& visitor) override;
    std::optional<std::string> vs(const NPC& target) const override;
};

