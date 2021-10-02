#include "cave_screen.h"

CaveScreen::CaveScreen() : caves_(8675309), player_(128, 112), timer_(250) {
  caves_.generate();
  fx_ = caves_.startx();
  fy_ = caves_.starty();
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
    fx_ = caves_.startx();
    fy_ = caves_.starty();
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

  // TODO handle entrance and exit

  return true;
}

void CaveScreen::draw(Graphics& graphics) const {
  caves_.cave(fx_, fy_).draw(graphics);
  player_.draw(graphics);
}
