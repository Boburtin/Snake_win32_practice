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

inline constexpr int BASE_SPEED = 120;

enum class G_Status { PLAYING, GAME_OVER, PAUSED };
enum class T_Type { FREE, SNAKE };
enum class M_Paused { RESUME, RESTART, QUIT };
enum class M_Home { SOLO, MULTI, SCORES, SETTINGS, EXIT };

struct PVec2 {
    int x;
    int y;
    PVec2 operator+(const PVec2 &other) const { return {x + other.x, y + other.y}; }
    bool operator==(const PVec2 &other) const { return x == other.x && y == other.y; }
    int index() const { return y * COLS + x; }
};

struct Snake {
    PVec2 dir = {1, 0};
    PVec2 pDir = {1, 0};
    PVec2 body[TOTAL_TILES] = {{START_X, START_Y}};
    int head = 0;
    int len = 1;
    PVec2 next() const { return body[head] + dir; }
};

struct GameData {
    T_Type board[TOTAL_TILES] {T_Type::FREE};
    G_Status gState = G_Status::PLAYING;
    M_Paused mOption = M_Paused::RESUME;
    M_Home mHome = M_Home::SOLO;
    PVec2 food;
    int level = 1;
    bool gonnaDie(PVec2 p) const {
        return ((p.x < 0 || p.x >= COLS) || (p.y < 0 || p.y >= ROWS) || board[p.index()] == T_Type::SNAKE);
    }
    void reset(Snake &snake, PVec2 (*fn)()) {
        for (auto &t : board)
            t = T_Type::FREE;
        board[START_INDEX] = T_Type::SNAKE;
        gState = G_Status::PLAYING;
        mOption = M_Paused::RESUME;
        food = fn();
        snake = Snake {};
    }
    void pause() { gState = G_Status::PAUSED; }
    void cont() {
        gState = G_Status::PLAYING;
        mOption = M_Paused::RESUME;
    }
};

struct Settings {
    wchar_t username[32];
    int maxLevel;
    int maxLen;
};
