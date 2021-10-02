#include "cave_screen.h"

CaveScreen::CaveScreen() : caves_(8675309), player_(128, 112), timer_(250) {
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

  // player_.update(caves_[0], elapsed);

  return true;
}

void CaveScreen::draw(Graphics& graphics) const {
  caves_.draw(graphics);
  // player_.draw(graphics);
}
