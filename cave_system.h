#pragma once

#include <vector>
#include <random>

#include "cave_floor.h"

class CaveSystem {
  public:

    CaveSystem(unsigned long seed);

    CaveFloor& floor() { return floors_.at(current_floor_); }
    const CaveFloor& floor() const { return floors_.at(current_floor_); }
    size_t depth() const { return current_floor_; }

    void up();
    void down();

  private:

    size_t current_floor_;
    std::vector<CaveFloor> floors_;
    std::mt19937 rng_;

};
