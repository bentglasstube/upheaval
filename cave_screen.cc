#include "cave_screen.h"

CaveScreen::CaveScreen() : caves_(8675309), player_(256, 224), shuffle_timer_(60000) {
  caves_.generate();
  move_to(caves_.entrance());

  // TODO set player to start position
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
    caves_.generate();
    move_to(caves_.entrance());
  }

  player_.update(caves_.cave(fx_, fy_), elapsed);

  const Rect p = player_.collision_box();

  // TODO screen transitions

  const int height = Cave::pixel_height();
  const int width = Cave::pixel_width();
  if (p.left < 1) {
    --fx_;
    player_.set_position(width - Config::kTileSize - 1, player_.y());
  } else if (p.right > width - 1) {
    ++fx_;
    player_.set_position(Config::kTileSize + 1, player_.y());
  } else if (p.top < 1) {
    --fy_;
    player_.set_position(player_.x(), height - Config::kTileSize - 1);
  } else if (p.bottom > height - 1) {
    ++fy_;
    player_.set_position(player_.x(), Config::kTileSize / 2 - 1);
  }

  if (fy_ < 0) {
    caves_.generate();
    move_to(caves_.entrance());
  } else if (fy_ > 3) {
    caves_.generate();
    move_to(caves_.exit());
  }

  shuffle_timer_ -= elapsed;
  if (shuffle_timer_ < 0) {
    // TODO earthquake effect
    caves_.shuffle_rooms(fx_, fy_);
    shuffle_timer_ = 60000;
  }

  return true;
}

void CaveScreen::draw(Graphics& graphics) const {
  caves_.cave(fx_, fy_).draw(graphics);
  player_.draw(graphics);

  /* caves_.draw(graphics); */
  /* int px = 4 * (fx_ * Cave::kMapWidth + player_.x() / Config::kTileSize); */
  /* int py = 4 * (fy_ * Cave::kMapHeight + player_.y() / Config::kTileSize); */
  /* graphics.draw_rect({px, py}, {px + 3, py + 3}, 0xd8ff00ff, true); */
}

void CaveScreen::move_to(const CaveFloor::Position& p) {
  fx_ = p.cave_x;
  fy_ = p.cave_y;
  player_.set_position(p.player_x, p.player_y);
}
