#pragma once

#include "observer.h"
#include <string>

class FileObserver : public Observer {
private:
    std::string filename;

public:
    explicit FileObserver(const std::string& file);
    void notify(const BattleEvent& event) const override;
};

