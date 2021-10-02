#include "cave_screen.h"

CaveScreen::CaveScreen() : cave_(8675309), timer_(250) {}

bool CaveScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  timer_ -= elapsed;
  if (timer_ < 0) {
    cave_.generate();
    timer_ += 250;
  }

  return true;
}

void CaveScreen::draw(Graphics& graphics) const {
  cave_.draw(graphics);
}
