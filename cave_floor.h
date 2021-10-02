#pragma once

#include <array>
#include <random>

#include "graphics.h"

#include "cave.h"
#include "direction.h"

class CaveFloor {
  public:
    CaveFloor(unsigned long seed, int floor);

    void draw(Graphics& graphics) const;
    void generate();

    Cave& cave(int x, int y);
    const Cave& cave(int x, int y) const;

    struct Position {
      int cave_x, cave_y;
      int player_x, player_y;
    };

    Position entrance() const { return entrance_; }
    Position exit() const { return exit_; }

    void shuffle_rooms(int x, int y);

  private:

    static constexpr int kLastFloor = 4;

    std::mt19937 rng_;
    int floor_;
    std::array<Cave, 16> caves_;
    std::unordered_set<int> visited_;
    Position entrance_, exit_;

    struct Path {
      int start, end;
      operator bool() const { return end > start; }
      int midpoint() const { return Config::kTileSize / 2 * (start + end); }
    };

    Path pick_path(int x, int y, Direction d);
    Path get_path(int x, int y, Direction d);
    void make_path(int x, int y, Direction d, Path p);
    Path join(int x, int y, Direction d, bool prefer_existing = true);
    void random_join(int x, int y);

    void path_to_entrance(int x, int y);
    void path_to_exit(int x, int y);
    void join_unvisited_rooms();
    void join_random_rooms(int count);
    Path join_existing(int x, int y, Direction d);
};
