#pragma once

inline constexpr int WIDTH = 800;
inline constexpr int HEIGHT = 600;
inline constexpr int TILESIZE = 20;
inline constexpr int COLS = WIDTH / TILESIZE;
inline constexpr int ROWS = HEIGHT / TILESIZE;
inline constexpr int TOTAL_TILES = ROWS * COLS;
inline constexpr int START_X = COLS / 2;
inline constexpr int START_Y = ROWS / 2;
inline constexpr int START_INDEX = START_Y * COLS + START_X;

enum class Gstate { PLAYING, GAME_OVER, PAUSED };
enum class Gtile { FREE, SNAKE };
enum class Gmode { SOLO, PVP };
enum class Moption { CONTINUE, RESTART, QUIT };

struct PVec2 {
  int x;
  int y;
  PVec2 operator+(const PVec2 &other) const { return {x + other.x, y + other.y}; }
  bool operator==(const PVec2 &other) const { return x == other.x && y == other.y; }
  int index() const { return y * COLS + x; }
};

struct OpaqueMenu {
    Moption opt;
};

struct Snake {
  PVec2 pdir;
  PVec2 dir;
  PVec2 body[TOTAL_TILES];
  int head;
  int len;
};

struct GameData {
  Gtile board[TOTAL_TILES];
  Gstate gState;
  PVec2 food;
};
