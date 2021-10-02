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
        enum Value : uint8_t { Open, Wall, Hole, Water, Lava, ExitUp, ExitDown, OOB, Keep };

        constexpr Tile() : value_(Wall) {}
        constexpr Tile(Value v) : value_(v) {}
        operator Value() const { return value_; }
        explicit operator bool() = delete;

        bool obstructs() const {
          switch (value_) {
            case Open:
            case ExitUp:
            case ExitDown:
              return false;
            default:
              return true;
          }
        }

        uint64_t color() const {
          switch (value_) {
            case Open:
              return 0x993311ff;
            case Wall:
              return 0xaaaaaaff;
            case Hole:
              return 0x000000ff;
            case Water:
              return 0x0000ffff;
            case Lava:
              return 0xff0000ff;
            case ExitUp:
              return 0xcc6622ff;
            case ExitDown:
              return 0x551100ff;
            default:
              return 0xff00ffff;
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

    static constexpr int kMapHeight = 28;
    static constexpr int kMapWidth = 32;

  private:

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
    void make_exit(int y, Tile t);
    void make_top_exit() { make_exit(0, Tile::ExitDown); }
    void make_bottom_exit() { make_exit(kMapHeight - 1, Tile::ExitUp); }
    void add_spice(Tile t);
    void add_hole();

    int flood_fill(int x, int y, Tile from, Tile to);
};
