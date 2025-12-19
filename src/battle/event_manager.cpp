#include "../../include/battle/event_manager.h"
#include "../../include/battle/observer.h"
#include "../../include/battle/battle_event.h"
#include <vector>
#include <algorithm>
#include <memory>

class EventManager::Impl {
public:
    std::vector<Observer*> observers;
    
    void subscribe(Observer* observer) {
        if (observer != nullptr) {
            observers.push_back(observer);
        }
    }
    
    void unsubscribe(Observer* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }
    
    void publish(const BattleEvent& event) const {
        for (auto* observer : observers) {
            if (observer != nullptr) {
                observer->notify(event);
            }
        }
    }
};

EventManager::EventManager() : pImpl(std::make_unique<Impl>()) {}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) noexcept = default;
EventManager& EventManager::operator=(EventManager&&) noexcept = default;

void EventManager::subscribe(Observer* observer) {
    pImpl->subscribe(observer);
}

void EventManager::unsubscribe(Observer* observer) {
    pImpl->unsubscribe(observer);
}

void EventManager::publish(const BattleEvent& event) const {
    pImpl->publish(event);
}

