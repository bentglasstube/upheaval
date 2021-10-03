#include "title_screen.h"

TitleScreen::TitleScreen() : text_("text.png"), backdrop_("title.png"), story_("story.png") {}

bool TitleScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  if (story_timer_ > 0) {
    story_timer_ -= elapsed;
    return !input.any_pressed();
  }

  if (input.key_pressed(Input::Button::Start)) return false;

  if (!dialog_) load_story_text();
  dialog_.update(elapsed);

  if (dialog_.done()) {
    story_timeout_ -= elapsed;
    if (story_timeout_ < 0) {
      ++story_text_;
      dialog_.dismiss();
    }
  }

  if (input.key_pressed(Input::Button::A)) {
    if (dialog_.done()) {
      ++story_text_;
      dialog_.dismiss();
    } else {
      dialog_.finish();
    }
  }

  return true;
}

void TitleScreen::draw(Graphics& graphics) const {
  if (story_timer_ > 0) {
    backdrop_.draw(graphics);
    text_.draw(graphics, "Press any key", graphics.width() / 2, graphics.height() - 32, Text::Alignment::Center);
  } else {
    story_.draw(graphics);
    dialog_.draw(graphics);
  }
}

void TitleScreen::load_story_text() {
  switch (story_text_) {
    case 0:
      dialog_.set_message("In my home village, there is a curse.  All\nthose who were cursed have turned to stone.\nI was the last living person.");
      break;

    case 1:
      dialog_.set_message("In order to break the curse, I have set out\nto find the magical Rorschart Amulet.  It is\nsaid to be hidden in a very unstable cave.\n\nNone who enter are ever seen again.");
      break;

    case 2:
      dialog_.set_message("Some have said they heard an incredible\nrumbling from the entrance and were too\nscared to enter.  Rumor has it that the\ncave's walls shift around you, causing you\nto become lost inside.");
      break;

    case 3:
      dialog_.set_message("I am my village's last hope.\n\n\nI must find the amulet.");
      break;

    default:
      story_timer_ = 15000;
      story_text_ = 0;
  }

  story_timeout_ = 10000;
}
