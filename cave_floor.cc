#include "cave_floor.h"

#include <unordered_set>

CaveFloor::CaveFloor(unsigned long seed) : rng_(seed) {
  generate();
}

void CaveFloor::draw(Graphics& graphics) const {
  for (int cy = 0; cy < 4; ++cy) {
    for (int cx = 0; cx < 4; ++cx) {
      const Cave& cave = caves_[cy * 4 + cx];

      for (int y = 0; y < Cave::kMapHeight; ++y) {
        for (int x = 0; x < Cave::kMapWidth; ++x) {
          const Cave::Tile t = cave.get_tile(x, y);
          const int dx = 4 * (cx * Cave::kMapWidth + x);
          const int dy = 4 * (cy * Cave::kMapHeight + y);
          graphics.draw_rect({dx, dy}, {dx + 3, dy + 3}, t.color(), true);
        }
      }
    }
  }
}

void CaveFloor::generate() {
  for (size_t i = 0; i < caves_.size(); ++i) {
    caves_[i].generate(rng_());
  }

  int x = std::uniform_int_distribution<int>(0, 3)(rng_);
  int y = 3;

  join(x, y, Direction::South);

  std::unordered_set<int> caves_seen;

  std::uniform_int_distribution<int> picker(0, 4);
  while (y >= 0) {
    const int dir = picker(rng_);
    caves_seen.insert(y * 4 + x);

    if (dir < 2) {
      if (x > 0) join(x--, y, Direction::West);
    } else if (dir < 4) {
      if (x < 3) join(x++, y, Direction::East);
    } else {
      join(x, y--, Direction::North);
    }
  }

  for (int i = 0; i < 16; ++i) {
    if (caves_seen.count(i) == 0) {
      const int ix = i % 4;
      const int iy = i / 4;
      random_join(ix, iy);
    }
  }

  std::uniform_int_distribution<int> coord_picker(0, 3);
  for (int i = 0; i < 5; ++i) {
    random_join(coord_picker(rng_), coord_picker(rng_));
  }
}

CaveFloor::Path CaveFloor::pick_path(int x, int y, Direction d) {
  const Cave& cave = caves_[y * 4 + x];
  int min = 99;
  int max = -1;

  if (d == Direction::North || d == Direction::South) {
    int iy = d == Direction::North ? 3 : Cave::kMapHeight - 4;
    while (min >= max) {
      for (int ix = 0; ix < Cave::kMapWidth; ++ix) {
        if (cave.get_tile(ix, iy) == Cave::Tile::Open) {
          if (ix < min) min = ix;
          if (ix > max) max = ix;
        }
      }
      d == Direction::North ? ++iy : --iy;
    }

    const int max_width = max - min > 9 ? 9 : max - min;
    const int width = std::uniform_int_distribution<int>(1, max_width)(rng_);
    const int start = std::uniform_int_distribution<int>(min, max - width)(rng_);
    return { start, start + width };
  } else {
    int ix = d == Direction::West ? 3 : Cave::kMapWidth - 4;
    while (min >= max) {
      for (int iy = 0; iy < Cave::kMapHeight; ++iy) {
        if (cave.get_tile(ix, iy) == Cave::Tile::Open) {
          if (iy < min) min = iy;
          if (iy > max) max = iy;
        }
      }
      d == Direction::West ? ++ix : --ix;
    }

    const int max_width = max - min > 4 ? 4 : max - min;
    const int width = std::uniform_int_distribution<int>(1, max_width)(rng_);
    const int start = std::uniform_int_distribution<int>(min, max - width)(rng_);
    return { start, start + width };
  }
}

void CaveFloor::make_path(int x, int y, Direction d, Path p) {
  if (x < 0 || x > 3 || y < 0 || y > 3) return;

  Cave& cave = caves_[y * 4 + x];
  if (d == Direction::North || d == Direction::South) {
    for (int ix = p.start; ix <= p.end; ++ix) {
      int iy = d == Direction::North ? 0 : Cave::kMapHeight - 1;
      while (cave.get_tile(ix, iy) == Cave::Tile::Wall) {
        cave.set_tile(ix, iy, Cave::Tile::Open);
        d == Direction::North ? ++iy : --iy;
      }
    }
  } else {
    for (int iy = p.start; iy <= p.end; ++iy) {
      int ix = d == Direction::West ? 0 : Cave::kMapWidth - 1;
      while (cave.get_tile(ix, iy) == Cave::Tile::Wall) {
        cave.set_tile(ix, iy, Cave::Tile::Open);
        d == Direction::West ? ++ix : --ix;
      }
    }
  }
}

void CaveFloor::join(int x, int y, Direction d) {
  const Path p = pick_path(x, y, d);
  make_path(x, y, d, p);
  switch (d) {
    case Direction::North: --y; break;
    case Direction::South: ++y; break;
    case Direction::West:  --x; break;
    case Direction::East:  ++x; break;
  }
  make_path(x, y, d.reverse(), p);
}

void CaveFloor::random_join(int x, int y) {
  std::uniform_int_distribution<int> picker(0, 3);
  while (true) {
    const int dir = picker(rng_);
    if (dir == 0) {
      if (x > 0) {
        join(x, y, Direction::West);
        break;
      }
    } else if (dir == 1) {
      if (x < 3) {
        join(x, y, Direction::East);
        break;
      }
    } else if (dir == 2) {
      if (y > 0) {
        join(x, y, Direction::North);
        break;
      }
    } else if (dir == 3) {
      if (y < 3) {
        join(x, y, Direction::South);
        break;
      }
    }
  }
}
