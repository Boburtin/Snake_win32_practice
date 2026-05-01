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

struct Snake {
  PVec2 dir;
  PVec2 pDir;
  PVec2 body[TOTAL_TILES]; 
  int head;
  int len;
  void setDir() { dir = pDir; }
  PVec2 next() { return body[head] + dir; }
  Snake() {
    body[0] = {START_X, START_Y};
    dir = {1, 0};
    pDir = {1, 0};
    head = 0;
    len = 1;
  }
};

struct GameData {
  Gtile board[TOTAL_TILES] {Gtile::FREE};
  Gstate gState;
  PVec2 food;
  bool notFree(PVec2 p) {
    return ((p.x < 0 || p.x >= COLS) || (p.y < 0 || p.y >= ROWS) ||
            board[p.index()] == Gtile::SNAKE);
  }
  void cleanTiles() {
    for (auto &t : board)
      t = Gtile::FREE;
  }
};
