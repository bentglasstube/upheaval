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
        enum Value : uint8_t { Open, Wall, Hole, Water, Lava, ExitUp, ExitDown, OOB, Keep, Amulet, ExAmulet };

        constexpr Tile() : value_(Wall) {}
        constexpr Tile(Value v) : value_(v) {}
        operator Value() const { return value_; }
        explicit operator bool() = delete;

        bool obstructs() const {
          switch (value_) {
            case Open:
            case ExitUp:
            case ExitDown:
            case ExAmulet:
              return false;
            default:
              return true;
          }
        }

        bool occludes() const {
          switch (value_) {
            case Wall:
            case OOB:
              return true;
            default:
              return false;
          }
        }

        uint64_t color() const {
          switch (value_) {
            case Open:
            case ExAmulet:
              return 0x993311ff;
            case Wall:
              return 0xaaaaaaff;
            case Hole:
              return 0x222222ff;
            case Water:
              return 0x0000ffff;
            case Lava:
              return 0xff0000ff;
            case ExitUp:
              return 0xcc6622ff;
            case ExitDown:
              return 0x551100ff;
            case Amulet:
              return 0x9911ffff;
            default:
              return 0xff00ffff;
          }
        }

      private:
        Value value_;
    };

    struct Cell {
      Tile tile = Tile::OOB;
      bool visible = false, seen = false;
    };

    Cave();
    void draw(Graphics& graphics, int xo, int yo) const;
    void generate(unsigned long seed);

    bool box_walkable(const Rect& r) const;
    bool walkable(double px, double py) const;

    void calculate_visibility(int x, int y, int max);
    Tile get_tile(int x, int y) const;
    void take_amulet(int x, int y);

    friend class CaveFloor;

    static constexpr int pixel_width() { return kMapWidth * Config::kTileSize; }
    static constexpr int pixel_height() { return kMapHeight * Config::kTileSize; }

    static constexpr int kMapHeight = 28;
    static constexpr int kMapWidth = 32;

  private:

    std::mt19937 rng_;
    std::array<Cell, kMapHeight * kMapWidth> cells_;

    struct Shadow {
      double start, end;
      bool contains(const Shadow& other) const;
    };

    class ShadowLine {
      public:
        ShadowLine();
        bool is_shadowed(const Shadow& shadow) const;
        void add(const Shadow& shadow);

      private:
        std::vector<Shadow> shadows_;
    };

    int index(int x, int y) const { return y * kMapWidth + x; }
    void set_tile(int x, int y, Tile t);
    void set_visible(int x, int y, bool visible);
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
    void add_treasure();
    bool has_treasure() const;

    int flood_fill(int x, int y, Tile from, Tile to);
};
