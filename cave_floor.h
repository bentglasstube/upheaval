#pragma once

#include <array>
#include <random>

#include "graphics.h"

#include "cave.h"
#include "direction.h"

class CaveFloor {
  public:
    CaveFloor(unsigned long seed);

    void draw(Graphics& graphics) const;
    void generate();

    Cave& cave(int x, int y);
    const Cave& cave(int x, int y) const;

    int startx() const { return sx_; }
    int starty() const { return sy_; }

  private:

    std::mt19937 rng_;
    std::array<Cave, 16> caves_;
    int sx_, sy_;

    struct Path { int start, end; };

    Path pick_path(int x, int y, Direction d);
    void make_path(int x, int y, Direction d, Path p);
    void join(int x, int y, Direction d);
    void random_join(int x, int y);
};
