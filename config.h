#pragma once

#include "game.h"

class Config : public Game::Config {
  public:
    Config();

    static constexpr int kTileSize = 16;
};

static const Config kConfig;
