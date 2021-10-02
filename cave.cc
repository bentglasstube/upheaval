#include "cave.h"

#include <cassert>
#include <stack>

#include "config.h"

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
  return !get_tile((int)(px / Config::kTileSize), (int)(py / Config::kTileSize)).obstructs();
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

  std::stack<int> stack;
  stack.push(index(x, y));
  while (!stack.empty()) {

    const int p = stack.top();
    stack.pop();

    const int px = p % kMapWidth;
    const int py = p / kMapWidth;

    if (get_tile(px, py) == Tile::Open) {
      set_tile(px, py, Tile::OOB);
      stack.push(index(px + 1, py));
      stack.push(index(px - 1, py));
      stack.push(index(px, py + 1));
      stack.push(index(px, py - 1));
    }
  }

  int count = 0;
  for (int fy = 0; fy < kMapHeight; ++fy) {
    for (int fx = 0; fx < kMapWidth; ++fx) {
      const Tile t = get_tile(fx, fy);
      if (t == Tile::OOB) {
        set_tile(fx, fy, Tile::Open);
        ++count;
      } else if (t == Tile::Open) {
        set_tile(fx, fy, Tile::Wall);
      }
    }
  }

  return count * 2 > kMapWidth * kMapHeight;
}
