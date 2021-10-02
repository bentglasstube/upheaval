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

    void shuffle_rooms(int x, int y);

  private:

    std::mt19937 rng_;
    std::array<Cave, 16> caves_;
    int sx_, sy_;
    std::unordered_set<int> visited_;

    struct Path {
      int start, end;
      operator bool() const { return end > start; }
    };

    Path pick_path(int x, int y, Direction d);
    Path get_path(int x, int y, Direction d);
    void make_path(int x, int y, Direction d, Path p);
    void join(int x, int y, Direction d, bool prefer_existing = true);
    void random_join(int x, int y);

    void path_to_entrance(int x, int y);
    void path_to_exit(int x, int y);
    void join_unvisited_rooms();
    void join_random_rooms(int count);
    bool join_existing(int x, int y, Direction d);
};
