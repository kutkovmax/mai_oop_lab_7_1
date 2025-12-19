#pragma once

#include "observer.h"
#include "battle_event.h"
#include <memory>

// PIMPL pattern для инкапсуляции и уменьшения связности
class EventManager {
public:
    EventManager();
    ~EventManager();
    
    // Запрет копирования и присваивания (Rule of Five)
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;
    EventManager(EventManager&&) noexcept;
    EventManager& operator=(EventManager&&) noexcept;
    
    // Command: изменение состояния
    void subscribe(Observer* observer);
    void unsubscribe(Observer* observer);
    
    // Command: уведомление наблюдателей
    void publish(const BattleEvent& event) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

