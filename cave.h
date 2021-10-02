#pragma once

#include <array>
#include <random>

#include "graphics.h"
#include "rect.h"

#include "config.h"

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
              return 0xff0000ff;
          }
        }

      private:
        Value value_;
    };

    Cave();
    void draw(Graphics& graphics) const;
    void generate(unsigned long seed);

    bool box_walkable(const Rect& r) const;
    bool walkable(double px, double py) const;

    friend class CaveFloor;

    static constexpr int pixel_width() { return kMapWidth * Config::kTileSize; }
    static constexpr int pixel_height() { return kMapHeight * Config::kTileSize; }

  private:

    static constexpr int kMapHeight = 28;
    static constexpr int kMapWidth = 32;

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
