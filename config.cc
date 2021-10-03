#include "config.h"

Config::Config() : Game::Config() {
  graphics.title = "Upheaval";
  graphics.width = 512;
  graphics.height = 480;
  graphics.intscale = 3;
  graphics.fullscreen = false;
}
