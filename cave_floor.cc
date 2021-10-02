#include "cave_floor.h"

#include <cassert>
#include <unordered_set>

CaveFloor::CaveFloor(unsigned long seed, int floor) : rng_(seed), floor_(floor) {}

void CaveFloor::draw(Graphics& graphics) const {
  for (int cy = 0; cy < 4; ++cy) {
    for (int cx = 0; cx < 4; ++cx) {
      const Cave& cave = caves_[cy * 4 + cx];

      for (int y = 0; y < Cave::kMapHeight; ++y) {
        for (int x = 0; x < Cave::kMapWidth; ++x) {
          const Cave::Tile t = cave.get_tile(x, y);
          const int dx = 4 * (cx * Cave::kMapWidth + x);
          const int dy = 4 * (cy * Cave::kMapHeight + y);
          graphics.draw_rect({dx, dy}, {dx + 4, dy + 4}, t.color(), true);
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

  const Path en = join(x, y, Direction::South);
  entrance_ = { x, y, Config::kTileSize * (en.start + en.end) / 2, 439 };
  cave(x, y).make_bottom_exit();

  visited_.clear();

  path_to_exit(x, y);
  join_unvisited_rooms();
  join_random_rooms(5);
}

Cave& CaveFloor::cave(int x, int y) {
  assert(x >= 0 && x < 4);
  assert(y >= 0 && y < 4);
  return caves_[4 * y + x];
}

const Cave& CaveFloor::cave(int x, int y) const {
  assert(x >= 0 && x < 4);
  assert(y >= 0 && y < 4);
  return caves_[4 * y + x];
}

CaveFloor::Path CaveFloor::pick_path(int x, int y, Direction d) {
  const Cave& cave = caves_[y * 4 + x];
  int min = 99;
  int max = -1;

  if (d == Direction::North || d == Direction::South) {
    int iy = d == Direction::North ? 3 : Cave::kMapHeight - 4;
    while (min >= max) {
      for (int ix = 2; ix < Cave::kMapWidth - 2; ++ix) {
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
      for (int iy = 2; iy < Cave::kMapHeight - 2; ++iy) {
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

CaveFloor::Path CaveFloor::get_path(int x, int y, Direction d) {
  const Cave& cave = caves_[y * 4 + x];
  int min = 99;
  int max = -1;

  if (d == Direction::North || d == Direction::South) {
    int iy = d == Direction::North ? 0 : Cave::kMapHeight - 1;
    for (int ix = 0; ix < Cave::kMapWidth; ++ix) {
      if (!cave.get_tile(ix, iy).obstructs()) {
        if (ix < min) min = ix;
        if (ix > max) max = ix;
      }
    }
    return { min, max };
  } else {
    int ix = d == Direction::West ? 0 : Cave::kMapWidth - 1;
    for (int iy = 0; iy < Cave::kMapHeight; ++iy) {
      if (!cave.get_tile(ix, iy).obstructs()) {
        if (iy < min) min = iy;
        if (iy > max) max = iy;
      }
    }
    return { min, max };
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

CaveFloor::Path CaveFloor::join(int x, int y, Direction d, bool prefer_existing) {
  if (prefer_existing) {
    const Path ep = join_existing(x, y, d);
    if (ep) return ep;
  }

  const Path p = pick_path(x, y, d);
  make_path(x, y, d, p);
  switch (d) {
    case Direction::North: --y; break;
    case Direction::South: ++y; break;
    case Direction::West:  --x; break;
    case Direction::East:  ++x; break;
  }
  make_path(x, y, d.reverse(), p);
  return p;
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

void CaveFloor::shuffle_rooms(int keep_x, int keep_y) {
  for (size_t i = 0; i < caves_.size(); ++i) {
    int ix = i % 4;
    int iy = i / 4;
    if (ix != keep_x || iy != keep_y) caves_[i].generate(rng_());
  }

  join_existing(keep_x, keep_y, Direction::North);
  join_existing(keep_x, keep_y, Direction::East);
  join_existing(keep_x, keep_y, Direction::South);
  join_existing(keep_x, keep_y, Direction::West);

  visited_.clear();
  path_to_entrance(keep_x, keep_y);
  path_to_exit(keep_x, keep_y);

  join_unvisited_rooms();
  join_random_rooms(5);
}

void CaveFloor::path_to_entrance(int x, int y) {
  std::uniform_int_distribution<int> picker(0, 4);
  while (y <= 3) {
    const int dir = get_path(x, y, Direction::South) ? 4 : picker(rng_);
    visited_.insert(y * 4 + x);

    if (dir < 2) {
      if (x > 0) join(x--, y, Direction::West, true);
    } else if (dir < 4) {
      if (x < 3) join(x++, y, Direction::East, true);
    } else {
      const Path p = join(x, y, Direction::South, true);
      entrance_ = { x, y, p.midpoint(), Cave::pixel_height() - Config::kTileSize - 1 };
      ++y;
    }
  }

  cave(x, y - 1).make_bottom_exit();
}

void CaveFloor::path_to_exit(int x, int y) {
  std::uniform_int_distribution<int> picker(0, 4);
  while (y >= 0) {
    visited_.insert(y * 4 + x);

    if (cave(x, y).has_treasure()) return;
    const int dir = get_path(x, y, Direction::North) ? 4 : picker(rng_);

    if (dir < 2) {
      if (x > 0) join(x--, y, Direction::West, true);
    } else if (dir < 4) {
      if (x < 3) join(x++, y, Direction::East, true);
    } else {
      if (y == 0 && floor_ == kLastFloor) {
        cave(x, y).add_treasure();
      } else {
        const Path p = join(x, y, Direction::North, true);
        exit_ = { x, y, p.midpoint(), Config::kTileSize / 2 - 1 };
      }
      --y;
    }
  }

  cave(x, y + 1).make_top_exit();
}

void CaveFloor::join_unvisited_rooms() {
  for (size_t i = 0; i < caves_.size(); ++i) {
    if (visited_.count(i) == 0) {
      random_join(i % 4, i / 4);
    }
  }
}

void CaveFloor::join_random_rooms(int count) {
  std::uniform_int_distribution<int> coord_picker(0, 3);
  for (int i = 0; i < count; ++i) {
    random_join(coord_picker(rng_), coord_picker(rng_));
  }
}

CaveFloor::Path CaveFloor::join_existing(int x, int y, Direction d) {
  const Path p = get_path(x, y, d);
  if (p) {
    switch (d) {
      case Direction::North: --y; break;
      case Direction::South: ++y; break;
      case Direction::West:  --x; break;
      case Direction::East:  ++x; break;
    }
    make_path(x, y, d.reverse(), p);
  }

  return p;
}
