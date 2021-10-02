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
    ":config",
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
    ":cave",
    ":config",
  ],
)
