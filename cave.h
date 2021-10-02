#pragma once

#include <array>
#include <random>

#include "graphics.h"
#include "rect.h"

class Cave {
  public:

    class Tile {
      public:
        enum Value : uint8_t { Open, Wall, OOB };

        constexpr Tile() : value_(Wall) {}
        constexpr Tile(Value v) : value_(v) {}
        operator Value() const { return value_; }
        explicit operator bool() = delete;

        bool obstructs() const {
          switch (value_) {
            case Open:
              return false;
            default:
              return true;
          }
        }

        uint64_t color() const {
          switch (value_) {
            case Open:
              return 0x111111ff;
            case Wall:
              return 0xffffffff;
            default:
              return 0xd8ff00ff;
          }
        }

      private:
        Value value_;
    };

    Cave(unsigned int seed);
    void draw(Graphics& graphics) const;
    void generate();

    bool box_walkable(const Rect& r) const;
    bool walkable(double px, double py) const;

  private:

    static constexpr int kTileSize = 4;
    static constexpr int kMapHeight = 224 / kTileSize;
    static constexpr int kMapWidth = 256 / kTileSize;

    std::mt19937 rng_;
    std::array<Tile, kMapHeight * kMapWidth> tiles_;

    int index(int x, int y) const { return y * kMapWidth + x; }
    Tile get_tile(int x, int y) const;
    void set_tile(int x, int y, Tile t);
    int walls_near(int x, int y, int r) const;

    void fill_random(float rate);
    void smooth(bool fill_open);
    void fill_borders();
    bool keep_only_biggest();

};
