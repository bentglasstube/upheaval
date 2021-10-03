#pragma once

#include "backdrop.h"
#include "screen.h"
#include "text.h"

#include "title_screen.h"

class WinScreen : public Screen {
  public:

    WinScreen(double time);

    bool update(const Input&, Audio&, unsigned int) override;
    void draw(Graphics& graphics) const override;

    Screen* next_screen() const override { return new TitleScreen(); }
    std::string get_music_track() const override { return "title.ogg"; }

  private:

    std::string time_;
    Text text_;
    Backdrop backdrop_;

};
