#ifndef SNAKE_H
#define SNAKE_H

inline constexpr int WIDTH = 800;
inline constexpr int HEIGHT = 600;
inline constexpr int TILESIZE = 20;
inline constexpr int COLS = WIDTH / TILESIZE;
inline constexpr int ROWS = HEIGHT / TILESIZE;
inline constexpr int TOTAL_TILES = ROWS * COLS;
inline constexpr int START_X = COLS / 2;
inline constexpr int START_Y = ROWS / 2;
inline constexpr int START_INDEX = START_Y * COLS + START_X;

enum class GameState { PLAYING, GAME_OVER, PAUSED, HOME_MENU };
enum class HomeMenu { PLAY, MULTI, TOGGLE, QUIT };
enum class PauseMenu { RESUME, RESTART, BACK, QUIT };
enum class GameOverMenu { RESTART, BACK, QUIT };
enum class TileType { FREE, SNAKE };

struct PVec2 {
    int x;
    int y;
    PVec2 operator+(const PVec2 &other) const { return { x + other.x, y + other.y }; }
    bool operator==(const PVec2 &other) const { return x == other.x && y == other.y; }
    int index() const { return y * COLS + x; }
};

struct Snake {
    PVec2 dir = { 1, 0 };
    PVec2 pDir = { 1, 0 };
    PVec2 body[TOTAL_TILES] = { { START_X, START_Y } };
    int head = 0;
    int len = 1;
    PVec2 next() const { return body[head] + dir; }
};

struct GameData {
    TileType board[TOTAL_TILES] { TileType::FREE };
    GameState gameState = GameState::HOME_MENU;
    PauseMenu pauseMenu = PauseMenu::RESUME;
    HomeMenu homeMenu = HomeMenu::PLAY;
    GameOverMenu gameOverMenu = GameOverMenu::RESTART;
    PVec2 food;
    int level = 1;
    int score = 0;
    bool isDead(PVec2 p) const {
        return ((p.x < 0 || p.x >= COLS) || (p.y < 0 || p.y >= ROWS) || board[p.index()] == TileType::SNAKE);
    }
    void reset(int i, PVec2 (*fn)(), Snake &snake) {
        for (auto &t : board) t = TileType::FREE;
        board[i] = TileType::SNAKE;
        food = fn();
        gameState = GameState::PLAYING;
        snake.len = 1;
        snake.body[0] = snake.body[snake.head];
        snake.head = 0;
    }
    void reset(Snake &snake, PVec2 (*fn)()) {
        for (auto &t : board) t = TileType::FREE;
        level = 1;
        score = 0;
        gameState = GameState::PLAYING;
        food = fn();
        snake = Snake {};
    }
    void pause() { gameState = GameState::PAUSED; }
    void resume() { gameState = GameState::PLAYING; }
};

struct Settings {
    wchar_t username[32];
    UINT highScore = 0U;
};

#endif