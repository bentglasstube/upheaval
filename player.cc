#include "player.h"

Player::Player(int x, int y) :
  x_(x), y_(y),
  facing_(Direction::North),
  state_(State::Waiting) {}

void Player::move(Direction dir) {
  state_ = State::Walking;
  facing_ = dir;
}

void Player::stop() {
  state_ = State::Waiting;
}

void Player::update(const Cave& cave, unsigned int elapsed) {
  // generic

  if (state_ == State::Walking) {
    const double delta = kSpeed * elapsed;
    switch (facing_) {
      case Direction::North:
        move_if_possible(cave, 0, -delta);
        break;

      case Direction::East:
        move_if_possible(cave, delta, 0);
        break;

      case Direction::South:
        move_if_possible(cave, 0, delta);
        break;

      case Direction::West:
        move_if_possible(cave, -delta, 0);
        break;
    }
  }
}

void Player::draw(Graphics& graphics) const {
  const Rect r = collision_box();
  graphics.draw_rect({(int)r.left, (int)r.top}, {(int)r.right + 1, (int)r.bottom + 1}, 0xd8ff00ff, true);
}

Rect Player::collision_box() const {
  return { x_ - 2, y_ - 2, x_ + 1, y_ + 1 };
}

bool Player::move_if_possible(const Cave& cave, double dx, double dy) {
  x_ += dx;
  y_ += dy;

  if (collision(cave)) {
    x_ -= dx;
    y_ -= dy;
    return false;
  } else {
    return true;
  }
}

bool Player::collision(const Cave& cave) const {
  return !cave.box_walkable(collision_box());
}
