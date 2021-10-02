#pragma once

#include "graphics.h"
#include "rect.h"

#include "cave.h"
#include "direction.h"

class Player {
  public:

    enum class State { Waiting, Walking };

    Player(int x, int y);

    void move(Direction dir);
    void stop();

    void update(const Cave& cave, unsigned int elapsed);
    void draw(Graphics& graphics) const;

    Rect draw_box() const;
    Rect collision_box() const;

    double x() const { return x_; }
    double y() const { return y_; }
    void set_position(int x, int y);

  private:

    static constexpr double kSpeed = 0.075;

    double x_, y_;
    Direction facing_;
    State state_;

    bool move_if_possible(const Cave& cave, double dx, double dy);
    bool collision(const Cave& cave) const;

};
