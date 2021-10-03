#pragma once

#include "backdrop.h"
#include "screen.h"
#include "text.h"

#include "cave_screen.h"

class TitleScreen : public Screen {
  public:

    TitleScreen();

    bool update(const Input&, Audio&, unsigned int) override;
    void draw(Graphics&) const override;

    Screen* next_screen() const override { return new CaveScreen(); }

  private:

    Text text_;
    Backdrop backdrop_;

};
