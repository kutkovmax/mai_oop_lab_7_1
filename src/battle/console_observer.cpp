#include "../../include/battle/console_observer.h"
#include "../../include/battle/battle_event.h"
#include <iostream>

void ConsoleObserver::notify(const BattleEvent& event) const {
    std::cout << "[Console] Action: " << event.action << std::endl;
}

