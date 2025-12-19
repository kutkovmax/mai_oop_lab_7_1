#pragma once

// Forward declarations для уменьшения связности
class Druid;
class Orc;
class Squirrel;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(Druid& druid) = 0;
    virtual void visit(Orc& orc) = 0;
    virtual void visit(Squirrel& squirrel) = 0;
};

