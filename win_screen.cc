#include "win_screen.h"

#include <cmath>
#include <sstream>

WinScreen::WinScreen(double time) : text_("text.png"), backdrop_("winner.png") {
  std::ostringstream ts;

  const int s = (int)std::ceil(time) % 60;
  const int m = ((int)time / 60) % 60;
  const int h = ((int)time / 3600);

  if (h > 0) {
    ts << h << ":";
    if (m < 10) ts << "0";
  }

  ts << m << ":";
  if (s < 10) ts << "0";
  ts << s;

  time_ = ts.str();
}

bool WinScreen::update(const Input& input, Audio&, unsigned int) {
  return !input.any_pressed();;
}

void WinScreen::draw(Graphics& graphics) const {
  backdrop_.draw(graphics);

  text_.draw(graphics, "Congratulations", 112, 16, Text::Alignment::Center);
  text_.draw(graphics, "You have retrieved the", 16, 48);
  text_.draw(graphics, "Rorschart Amulet and saved", 16, 64);
  text_.draw(graphics, "your village.", 16, 80);
  text_.draw(graphics, "Play time:", 16, 112);

  text_.draw(graphics, time_, 208, 112, Text::Alignment::Right);
}
