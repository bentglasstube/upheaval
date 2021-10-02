package(default_visibility = ["//visibility:public"])

cc_binary(
  name = "ld49",
  data = ["//content"],
  linkopts = [
    "-lSDL2",
    "-lSDL2_image",
    "-lSDL2_mixer",
  ],
  srcs = ["main.cc"],
  deps = [
    "@libgam//:game",
    ":screens",
  ],
)

cc_library(
  name = "screens",
  srcs = [
    "cave_screen.cc",
    "title_screen.cc",
  ],
  hdrs = [
    "cave_screen.h",
    "title_screen.h",
  ],
  deps = [
    "@libgam//:screen",
    "@libgam//:text",
    ":cave",
    ":player",
  ],
)

cc_library(
  name = "cave",
  srcs = ["cave.cc"],
  hdrs = ["cave.h"],
  deps = [
    "@libgam//:graphics",
    "@libgam//:rect",
  ],
)

cc_library(
  name = "player",
  srcs = ["player.cc"],
  hdrs = ["player.h"],
  deps = [
    "@libgam//:graphics",
    "@libgam//:rect",
    ":cave",
  ],
)
