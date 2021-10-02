#include "cave_system.h"

CaveSystem::CaveSystem(unsigned long seed) : current_floor_(0), rng_(seed) {
  floors_.emplace_back(rng_(), 0);
  floors_[0].generate();
}

void CaveSystem::up() {
  if (current_floor_ > 0) --current_floor_;
}

void CaveSystem::down() {
  ++current_floor_;
  while (current_floor_ >= floors_.size()) {
    const int f = floors_.size();
    floors_.emplace_back(rng_(), f);
    floors_[f].generate();
  }
}

