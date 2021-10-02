#pragma once

#include "graphics.h"
#include "rect.h"

#include "cave.h"

class Player {
  public:

    class Direction {
      public:
        enum Value : uint8_t { North, East, South, West };

        constexpr Direction() : value_(North) {}
        constexpr Direction(Value v) : value_(v) {}
        operator Value() const { return value_; }
        explicit operator bool() = delete;

        Direction reverse() const {
          switch (value_) {
            case North: return South;
            case South: return North;
            case East:  return West;
            case West:  return East;
            default:    return North;
          }
        }

      private:
        Value value_;
    };

    enum class State { Waiting, Walking };

    Player(int x, int y);

    void move(Direction dir);
    void stop();

    void update(const Cave& cave, unsigned int elapsed);
    void draw(Graphics& graphics) const;

    Rect draw_box() const;
    Rect collision_box() const;

  private:

    static constexpr double kSpeed = 0.05;

    double x_, y_;
    Direction facing_;
    State state_;

    bool move_if_possible(const Cave& cave, double dx, double dy);
    bool collision(const Cave& cave) const;

};
