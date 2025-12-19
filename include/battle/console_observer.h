#pragma once

#include "observer.h"

class ConsoleObserver : public Observer {
public:
    void notify(const BattleEvent& event) const override;
};

