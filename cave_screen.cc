#include "cave_screen.h"

#include <cmath>

CaveScreen::CaveScreen() : caves_(8675309), player_(256, 224), shuffle_timer_(60000) {
  move_to(caves_.floor().entrance());
}

bool CaveScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  if (input.key_held(Input::Button::Left)) {
    player_.move(Direction::West);
  } else if (input.key_held(Input::Button::Right)) {
    player_.move(Direction::East);
  } else if (input.key_held(Input::Button::Up)) {
    player_.move(Direction::North);
  } else if (input.key_held(Input::Button::Down)) {
    player_.move(Direction::South);
  } else {
    player_.stop();
  }

  if (input.key_pressed(Input::Button::A)) {
    player_.interact(caves_.floor().cave(fx_, fy_));
  }

  player_.update(caves_.floor().cave(fx_, fy_), elapsed);

  const Rect p = player_.collision_box();

  // TODO screen transitions

  const int height = Cave::pixel_height();
  const int width = Cave::pixel_width();
  const int ht = Config::kTileSize / 2;

  if (p.left < 1) {
    --fx_;
    player_.set_position(width - ht - 1, player_.y());
  } else if (p.right > width - 1) {
    ++fx_;
    player_.set_position(ht + 1, player_.y());
  } else if (p.top < 1) {
    --fy_;
    player_.set_position(player_.x(), height - ht - 1);
  } else if (p.bottom > height - 1) {
    ++fy_;
    player_.set_position(player_.x(), ht - 1);
  }

  if (input.key_pressed(Input::Button::Select)) cheater_mode_ = !cheater_mode_;

  if (fy_ < 0) {
    caves_.down();
    move_to(caves_.floor().entrance());
  } else if (fy_ > 3) {
    if (caves_.depth() == 0) {
      if (player_.has_amulet()) {
        // TODO win screen?
        return false;
      }
      player_.set_position(player_.x(), height - ht - 1);
      fy_ = 3;
    } else {
      caves_.up();
      move_to(caves_.floor().exit());
    }
  }

  shuffle_timer_ -= elapsed;
  if (shuffle_timer_ < 0) {
    caves_.floor().shuffle_rooms(fx_, fy_);
    shuffle_timer_ = 60000;
  }

  const int px = std::floor(player_.x() / Config::kTileSize);
  const int py = std::floor(player_.y() / Config::kTileSize);
  caves_.floor().cave(fx_, fy_).calculate_visibility(px, py, 10);

  return true;
}

void CaveScreen::draw(Graphics& graphics) const {
  const int shake = shake_amount();
  std::uniform_int_distribution<int> offset(-shake, shake);
  const int xo = shake > 0 ? offset(misc_rng_) : 0;
  const int yo = shake > 0 ? offset(misc_rng_) : 0;

  if (cheater_mode_) {
    caves_.floor().draw(graphics);
    int px = 4 * (fx_ * Cave::kMapWidth + player_.x() / Config::kTileSize);
    int py = 4 * (fy_ * Cave::kMapHeight + player_.y() / Config::kTileSize);
    graphics.draw_rect({px, py}, {px + 4, py + 4}, 0xd8ff00ff, true);
  } else {
    caves_.floor().cave(fx_, fy_).draw(graphics, xo, yo);
    player_.draw(graphics, xo, yo);
  }
}

void CaveScreen::move_to(const CaveFloor::Position& p) {
  fx_ = p.cave_x;
  fy_ = p.cave_y;
  player_.set_position(p.player_x, p.player_y);
}

int CaveScreen::shake_amount() const {
  if (shuffle_timer_ < 1000) {
    return 16 - 14 * shuffle_timer_ / 1000;
  } else if (shuffle_timer_ < 2000) {
    return 2;
  } else if (shuffle_timer_ < 3000) {
    return 1;
  } else {
    return 0;
  }
}
