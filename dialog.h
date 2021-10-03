#pragma once

#include <string>

#include "box.h"
#include "text.h"

class Dialog {
  public:

    Dialog();

    void set_message(const std::string& message);
    void update(unsigned int elapsed);
    void draw(Graphics& graphics) const;
    bool done() const { return index_ >= message_.length(); }
    void finish() { index_ = message_.length(); }
    void dismiss() { message_ = ""; }

    operator bool() const { return message_.length() > 0; }

  private:

    static constexpr int kRate = 50;

    Text text_;
    Box box_;
    std::string message_;
    int timer_;
    size_t index_;
};
