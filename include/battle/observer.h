#pragma once

#include "battle_event.h"

class Observer {
public:
    virtual ~Observer() = default;
    virtual void notify(const BattleEvent& event) const = 0;
};

