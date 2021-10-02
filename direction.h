#pragma once

class Direction {
  public:
    enum Value : uint8_t { North, East, South, West };

    constexpr Direction() : value_(North) {}
    constexpr Direction(Value v) : value_(v) {}
    operator Value() const { return value_; }
    explicit operator bool() = delete;

    Direction reverse() const {
      switch (value_) {
        case North: return South;
        case South: return North;
        case East:  return West;
        case West:  return East;
        default:    return North;
      }
    }

  private:
    Value value_;
};
