#pragma once

#include "screen.h"

#include "cave.h"

class CaveScreen : public Screen {
  public:

    CaveScreen();

    bool update(const Input&, Audio&, unsigned int) override;
    void draw(Graphics&) const override;

    Screen* next_screen() const override { return nullptr; }

  private:

    Cave cave_;
    int timer_;

};
