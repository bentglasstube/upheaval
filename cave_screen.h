#pragma once

#include "screen.h"

#include "cave_system.h"
#include "player.h"

class CaveScreen : public Screen {
  public:

    CaveScreen();

    bool update(const Input&, Audio&, unsigned int) override;
    void draw(Graphics&) const override;

    Screen* next_screen() const override;

  private:

    CaveSystem caves_;
    Player player_;
    int shuffle_timer_;
    double music_timer_, play_time_;
    int fx_, fy_;
    bool cheater_mode_ = false;

    mutable std::mt19937 misc_rng_;

    void move_to(const CaveFloor::Position& p);
    int shake_amount() const;

    void switch_music(Audio& audio) const;

};
