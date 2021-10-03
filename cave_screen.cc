#include "cave_screen.h"

#include <cmath>
#include <SDL2/SDL_mixer.h>

#include "util.h"

#include "win_screen.h"

CaveScreen::CaveScreen() :
  caves_(Util::random_seed()),
  player_(256, 224),
  shuffle_timer_(60000), music_timer_(0.0), play_time_(0.0) {
  move_to(caves_.floor().entrance());
}

bool CaveScreen::update(const Input& input, Audio& audio, unsigned int elapsed) {
  music_timer_ += elapsed / 1000.0f;
  if (music_timer_ > 8145732 / 48000.0) {
    music_timer_ -= (8145732 - 478444) / 48000.0;
  }

  play_time_ += elapsed / 1000.0f;

  if (!audio.music_playing()) {
    switch_music(audio);
    music_timer_ = 0;
  }

  if (dialog_) {

    dialog_.update(elapsed);
    if (input.key_pressed(Input::Button::A)) {
      if (dialog_.done()) {
        dialog_.dismiss();
      } else {
        dialog_.finish();
      }
    }

  } else {

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
      const Cave::Cell c = player_.interact(caves_.floor().cave(fx_, fy_));

      switch (c.tile) {
        case Cave::Tile::Open:
          dialog_.set_message("I searched the ground all about.\n         \nBut there found nothing.");
          break;

        case Cave::Tile::Wall:
          if (c.sprite == 8) {
            dialog_.set_message("This rock is strangely familiar.");
          } else {
            dialog_.set_message("The walls of the cave block my way.");
          }
          break;

        case Cave::Tile::Hole:
          dialog_.set_message("It's a hole.  I can't see the bottom.");
          break;

        case Cave::Tile::Water:
          dialog_.set_message("The water doesn't look very clean.  I had\nbetter leave it alone.");
          break;

        case Cave::Tile::Lava:
          dialog_.set_message("I guess I'm deep enough to find lava now.\nBest to stay away from it.");
          break;

        case Cave::Tile::ExitUp:
          dialog_.set_message("This leads back out of the cave.");
          break;

        case Cave::Tile::ExitDown:
          dialog_.set_message("This leads further into the cave.");
          break;

        case Cave::Tile::Chest:
          if (player_.has_amulet()) {
            dialog_.set_message("I already have what I came for.  There's no\nneed to be greedy.");
          } else {
            dialog_.set_message("At last, I've found the Rorschart Amulet!");
            player_.give_amulet();
            caves_.floor().cave(fx_, fy_).open_chest();
          }

          break;

        case Cave::Tile::OpenChest:
          dialog_.set_message("This chest is empty.");
          break;

        default: break;
      }
    }

    player_.update(caves_.floor().cave(fx_, fy_), elapsed);
  }

  const Rect p = player_.collision_box();

  const int height = Cave::pixel_height();
  const int width = Cave::pixel_width();
  const int ht = Config::kTileSize / 2;

  if (p.left < 1) {
    --fx_;
    player_.set_position(width - ht - 1, player_.y());
  } else if (p.right > width - 1) {
    ++fx_;
    player_.set_position(ht + 1, player_.y());
  } else if (p.top < 1) {
    --fy_;
    player_.set_position(player_.x(), height - ht - 1);
  } else if (p.bottom > height - 1) {
    ++fy_;
    player_.set_position(player_.x(), ht - 1);
  }

#ifndef NDEBUG
  if (input.key_pressed(Input::Button::Select)) cheater_mode_ = !cheater_mode_;
  if (input.key_pressed(Input::Button::B)) {
    player_.give_amulet();
    play_time_ += Util::random_seed() % 3600;
  }
#endif

  if (fy_ < 0) {
    caves_.down();
    move_to(caves_.floor().entrance());
    switch_music(audio);
  } else if (fy_ > 3) {
    if (caves_.depth() == 0) {
      if (player_.has_amulet()) {
        audio.play_music("title.ogg", true);
        return false;
      }

      dialog_.set_message("I can't leave until I find the amulet.");

      player_.set_position(player_.x(), height - ht - 1);
      fy_ = 3;
    } else {
      caves_.up();
      move_to(caves_.floor().exit());
      switch_music(audio);
    }
  }

  shuffle_timer_ -= elapsed;
  if (shuffle_timer_ < 0) {
    caves_.floor().shuffle_rooms(fx_, fy_);
    shuffle_timer_ = 60000;
  }

  const int px = std::floor(player_.x() / Config::kTileSize);
  const int py = std::floor(player_.y() / Config::kTileSize);
  const int visibility = 11 - 2 * caves_.depth() + (player_.has_amulet() ? 6 : 0);
  caves_.floor().cave(fx_, fy_).calculate_visibility(px, py, visibility);

  return true;
}

void CaveScreen::draw(Graphics& graphics) const {
  const int shake = shake_amount();
  std::uniform_int_distribution<int> offset(-shake, shake);
  const int xo = shake > 0 ? offset(misc_rng_) : 0;
  const int yo = shake > 0 ? offset(misc_rng_) : 0;

  if (cheater_mode_) {
    caves_.floor().draw(graphics);
    int px = 4 * (fx_ * Cave::kMapWidth + player_.x() / Config::kTileSize);
    int py = 4 * (fy_ * Cave::kMapHeight + player_.y() / Config::kTileSize);
    graphics.draw_rect({px, py}, {px + 4, py + 4}, 0xd8ff00ff, true);
  } else {
    caves_.floor().cave(fx_, fy_).draw(graphics, xo, yo);
    player_.draw(graphics, xo, yo);
  }

  if (dialog_) dialog_.draw(graphics);
}

void CaveScreen::move_to(const CaveFloor::Position& p) {
  fx_ = p.cave_x;
  fy_ = p.cave_y;
  player_.set_position(p.player_x, p.player_y);
}

int CaveScreen::shake_amount() const {
  if (shuffle_timer_ < 1000) {
    return 16 - 14 * shuffle_timer_ / 1000;
  } else if (shuffle_timer_ < 2000) {
    return 2;
  } else if (shuffle_timer_ < 3000) {
    return 1;
  } else {
    return 0;
  }
}

void CaveScreen::switch_music(Audio& audio) const {
  const std::string music = "funky-cave-" + std::to_string(caves_.depth()) + ".ogg";
  audio.play_music(music, true);
  Mix_SetMusicPosition(music_timer_);
}

Screen* CaveScreen::next_screen() const {
  if (player_.has_amulet()) {
    return new WinScreen(play_time_);
  } else {
    return new TitleScreen();
  }
}
