#include "cave_screen.h"

CaveScreen::CaveScreen() : cave_(8675309), player_(128, 112), timer_(250) {}

bool CaveScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  if (input.key_held(Input::Button::Left)) {
    player_.move(Player::Direction::West);
  } else if (input.key_held(Input::Button::Right)) {
    player_.move(Player::Direction::East);
  } else if (input.key_held(Input::Button::Up)) {
    player_.move(Player::Direction::North);
  } else if (input.key_held(Input::Button::Down)) {
    player_.move(Player::Direction::South);
  } else {
    player_.stop();
  }

  if (input.key_pressed(Input::Button::A)) {
    cave_.generate();
  }

  player_.update(cave_, elapsed);

  return true;
}

void CaveScreen::draw(Graphics& graphics) const {
  cave_.draw(graphics);
  player_.draw(graphics);
}
