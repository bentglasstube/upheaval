#include "cave.h"

#include <cassert>
#include <cmath>
#include <stack>

Cave::Cave() {}

void Cave::generate(unsigned long seed) {
  rng_.seed(seed);

  while (true) {
    fill_random(0.45);
    for (int i = 0; i < 5; ++i) smooth(true);
    for (int i = 0; i < 2; ++i) smooth(false);
    fill_borders();
    if (keep_only_biggest()) break;
  }

  for (int i = 0; i < 5; ++i) add_hole();
  add_spice(Tile::Water);

  // do more stuff
}

void Cave::draw(Graphics& graphics) const {
  for (int y = 0; y < kMapHeight; ++y) {
    for (int x = 0; x < kMapWidth; ++x) {
      const Tile t = get_tile(x, y);
      graphics.draw_rect(
          {x * Config::kTileSize, y * Config::kTileSize},
          {(x + 1) * Config::kTileSize, (y + 1) * Config::kTileSize},
          t.color(),
          true);
    }
  }
}

Cave::Tile Cave::get_tile(int x, int y) const {
  if (x < 0 || x >= kMapWidth) return Tile::OOB;
  if (y < 0 || y >= kMapHeight) return Tile::OOB;

  return tiles_[index(x, y)];
}

void Cave::set_tile(int x, int y, Tile t) {
  assert(x >= 0 && x < kMapWidth);
  assert(y >= 0 && y < kMapHeight);

  tiles_[index(x, y)] = t;
}

bool Cave::box_walkable(const Rect& r) const {
  return walkable(r.left, r.top) &&
    walkable(r.left, r.bottom) &&
    walkable(r.right, r.top) &&
    walkable(r.right, r.bottom);
}

bool Cave::walkable(double px, double py) const {
  const int x = std::floor(px / Config::kTileSize);
  const int y = std::floor(py / Config::kTileSize);
  return !get_tile(x, y).obstructs();
}

void Cave::fill_random(float rate) {
  std::uniform_real_distribution<float> r(0, 1);

  for (int y = 0; y < kMapHeight; ++y) {
    for (int x = 0; x < kMapWidth; ++x) {
      tiles_[index(x, y)] = { r(rng_) < rate ? Tile::Wall : Tile::Open };
    }
  }
}

int Cave::walls_near(int x, int y, int r) const {
  int count = 0;
  for (int iy = y - r; iy <= y + r; ++iy) {
    for (int ix = x - r; ix <= x + r; ++ix) {
      if (get_tile(ix, iy).obstructs()) ++count;
    }
  }

  return count;
}

void Cave::smooth(bool fill_open) {
  std::array<Tile, kMapHeight * kMapWidth> t;

  for (int y = 0; y < kMapHeight; ++y) {
    for (int x = 0; x < kMapWidth; ++x) {
      t[index(x, y)] = walls_near(x, y, 1) >= 5 || (fill_open && walls_near(x, y, 2) <= 2) ? Tile::Wall : Tile::Open;
    }
  }

  tiles_ = t;
}

void Cave::fill_borders() {
  for (int y = 0; y < kMapHeight; ++y) {
    set_tile(0, y, Tile::Wall);
    set_tile(kMapWidth - 1, y, Tile::Wall);
  }
  for (int x = 0; x < kMapWidth; ++x) {
    set_tile(x, 0, Tile::Wall);
    set_tile(x, kMapHeight - 1, Tile::Wall);
  }
}

bool Cave::keep_only_biggest() {
  int x = kMapWidth / 2;
  int y = kMapHeight / 2;

  while (get_tile(x, y) == Tile::Wall) {
    ++y;
    if (get_tile(x, y) == Tile::OOB) return false;
  }

  const int count = flood_fill(x, y, Tile::Open, Tile::Keep);
  if (count * 2 < kMapWidth * kMapHeight) return false;

  for (int fy = 0; fy < kMapHeight; ++fy) {
    for (int fx = 0; fx < kMapWidth; ++fx) {
      const Tile t = get_tile(fx, fy);
      if (t == Tile::Keep) {
        set_tile(fx, fy, Tile::Open);
      } else if (t == Tile::Open) {
        set_tile(fx, fy, Tile::Wall);
      }
    }
  }

  return true;
}

void Cave::make_exit(int y, Tile t) {
  for (int x = 0; x < kMapWidth; ++x) {
    if (get_tile(x, y) == Tile::Open) set_tile(x, y, t);
  }
}

void Cave::add_spice(Tile t) {
  std::uniform_int_distribution<int> rx(2, kMapWidth - 3);
  std::uniform_int_distribution<int> ry(2, kMapHeight - 3);

  int tries = 20;
  while (--tries > 0) {
    int x = rx(rng_);
    int y = ry(rng_);

    if (get_tile(x, y) == Tile::Wall) {
      flood_fill(x, y, Tile::Wall, t);
      bool undo = false;
      for (int ex = 0; ex < kMapWidth; ++ex) {
        if (get_tile(ex, 0) == t || get_tile(ex, kMapHeight - 1) == t) undo = true;
      }
      for (int ey = 0; ey < kMapHeight; ++ey) {
        if (get_tile(0, ey) == t || get_tile(kMapWidth - 1, ey) == t) undo = true;
      }

      if (undo) {
        flood_fill(x, y, t, Tile::Wall);
      } else {
        return;
      }
    }
  }
}

void Cave::add_hole() {
  std::uniform_int_distribution<int> rx(2, kMapWidth - 3);
  std::uniform_int_distribution<int> ry(2, kMapHeight - 3);

  int tries = 50;
  while (--tries > 0) {
    int x = rx(rng_);
    int y = ry(rng_);

    if (get_tile(x, y) == Tile::Wall) {
      const int count = flood_fill(x, y, Tile::Wall, Tile::Hole);
      if (count > 7) flood_fill(x, y, Tile::Hole, Tile::Wall);
    }
  }
}

int Cave::flood_fill(int x, int y, Tile from, Tile to) {
  int count = 0;
  std::stack<int> stack;
  stack.push(index(x, y));

  while (!stack.empty()) {
    const int p = stack.top();
    stack.pop();

    const int px = p % kMapWidth;
    const int py = p / kMapWidth;

    if (get_tile(px, py) == from) {
      ++count;
      set_tile(px, py, to);
      stack.push(index(px + 1, py));
      stack.push(index(px - 1, py));
      stack.push(index(px, py + 1));
      stack.push(index(px, py - 1));
    }
  }

  return count;
}
