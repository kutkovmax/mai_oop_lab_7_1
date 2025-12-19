#pragma once

#include <optional>
#include <string>
#include "../geometry/point.h"

// Forward declaration для уменьшения связности
class Visitor;

// Базовый класс NPC 
class NPC {
protected:
    std::string name;
    Point position;
    bool alive;

public:
    virtual ~NPC() = default;

    NPC(const std::string& name, const Point& position);

    // Qeries
    virtual std::string get_type() const = 0;
    virtual std::string get_name() const;
    virtual Point get_position() const;
    virtual bool is_alive() const;
    
    // Qery: проверка возможности убийства (не изменяет состояние)
    virtual std::optional<std::string> vs(const NPC& target) const = 0;

    // Command: принятие посетителя (Visitor pattern)
    virtual void accept(Visitor& visitor) = 0;
    
    // Command: убить NPC (изменяет состояние)
    void kill();
};
