#include "player.h"

#include <cmath>

#include "config.h"

Player::Player(int x, int y) :
  x_(x), y_(y),
  facing_(Direction::North),
  state_(State::Waiting),
  has_amulet_(false),
  sprites_("char.png", 4, 16, 24),
  timer_(0) {}

void Player::move(Direction dir) {
  state_ = State::Walking;
  facing_ = dir;
}

void Player::stop() {
  state_ = State::Waiting;
}

void Player::update(const Cave& cave, unsigned int elapsed) {
  if (state_ == State::Walking) {
    const double delta = kSpeed * elapsed;
    double dx = 0, dy = 0;
    switch (facing_) {
      case Direction::North: dy = -delta; break;
      case Direction::East:  dx = delta;  break;
      case Direction::South: dy = delta;  break;
      case Direction::West:  dx = -delta; break;
    }

    if (move_if_possible(cave, dx, dy)) {
      timer_ = (timer_ + elapsed) % (kAnimationTime * 4);
    }
  }
}

void Player::draw(Graphics& graphics, int xo, int yo) const {
  const int x = x_ - xo - Config::kTileSize / 2;
  const int y = y_ - yo - Config::kTileSize;
  sprites_.draw(graphics, sprite_number(), x, y);
}

Rect Player::draw_box() const {
  return {
    x_ - Config::kTileSize / 2,
    y_ - Config::kTileSize / 2,
    x_ + Config::kTileSize / 2,
    y_ + Config::kTileSize / 2 };
}

Rect Player::collision_box() const {
  return {
    x_ - Config::kTileSize / 2 + 1,
    y_,
    x_ + Config::kTileSize / 2 - 1,
    y_ + Config::kTileSize / 2 - 1 };
}

void Player::set_position(int x, int y) {
  x_ = x;
  y_ = y;
  stop();
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

void Player::interact(Cave& cave) {
  int px = std::floor(x_ / Config::kTileSize);
  int py = std::floor(y_ / Config::kTileSize);

  switch (facing_) {
    case Direction::North: --py; break;
    case Direction::East:  ++px; break;
    case Direction::South: ++py; break;
    case Direction::West:  --px; break;
  }

  if (cave.get_tile(px, py) == Cave::Tile::Chest) {
    cave.take_amulet(px, py);
    has_amulet_ = true;
  }
}

int Player::sprite_number() const {
  int d = 4 * facing_;
  if (state_ == State::Walking) {
    d += timer_ / kAnimationTime;
  }
  return d;
}
