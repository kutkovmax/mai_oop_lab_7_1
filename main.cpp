#include "include/game/game.h"
#include <iostream>

int main() {
    std::cout << "=== ЛАБОРАТОРНАЯ РАБОТА №7: АСИНХРОННОЕ ПРОГРАММИРОВАНИЕ ===\n";
    std::cout << "Создается " << Game::NUM_NPCS << " NPC на карте " 
              << Game::MAP_WIDTH << "x" << Game::MAP_HEIGHT << "\n";
    std::cout << "Игра продлится " << Game::GAME_DURATION_SECONDS << " секунд\n\n";
    
    Game game;
    game.start();
    
    return 0;
}
