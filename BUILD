package(default_visibility = ["//visibility:public"])

cc_binary(
  name = "upheaval",
  data = ["//content"],
  linkopts = [
    "-lSDL2",
    "-lSDL2_image",
    "-lSDL2_mixer",
  ],
  srcs = ["main.cc"],
  deps = [
    "@libgam//:game",
    ":config",
    ":screens",
  ],
)

cc_library(
  name = "screens",
  srcs = [
    "cave_screen.cc",
    "title_screen.cc",
    "win_screen.cc",
  ],
  hdrs = [
    "cave_screen.h",
    "title_screen.h",
    "win_screen.h",
  ],
  deps = [
    "@libgam//:backdrop",
    "@libgam//:screen",
    "@libgam//:text",
    "@libgam//:util",
    ":cave_system",
    ":dialog",
    ":player",
  ],
)

cc_library(
  name = "config",
  srcs = ["config.cc"],
  hdrs = ["config.h"],
  deps = [
    "@libgam//:game",
  ],
)

cc_library(
  name = "cave",
  srcs = ["cave.cc"],
  hdrs = ["cave.h"],
  deps = [
    "@libgam//:graphics",
    "@libgam//:rect",
    "@libgam//:spritemap",
    ":config",
  ],
)

cc_library(
  name = "player",
  srcs = ["player.cc"],
  hdrs = ["player.h"],
  deps = [
    "@libgam//:graphics",
    "@libgam//:rect",
    "@libgam//:spritemap",
    ":cave",
    ":config",
    ":direction",
  ],
)

cc_library(
  name = "cave_floor",
  srcs = ["cave_floor.cc"],
  hdrs = ["cave_floor.h"],
  deps = [
    "@libgam//:graphics",
    ":cave",
    ":direction",
  ],
)

cc_library(
  name = "direction",
  hdrs = ["direction.h"],
)

cc_library(
  name = "cave_system",
  srcs = ["cave_system.cc"],
  hdrs = ["cave_system.h"],
  deps = [
    ":cave_floor",
  ],
)

cc_library(
  name = "dialog",
  srcs = ["dialog.cc"],
  hdrs = ["dialog.h"],
  deps = [
    "@libgam//:box",
    "@libgam//:text",
  ],
)
