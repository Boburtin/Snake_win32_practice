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
inline constexpr float HPOINT = HEIGHT * .1f, HPOINT2 = HEIGHT * .2f, HPOINT3 = HEIGHT * .3f, HPOINT4 = HEIGHT * .4f,
                       HPOINT5 = HEIGHT * .5f, HPOINT6 = HEIGHT * .6f, HPOINT7 = HEIGHT * .7f, HPOINT8 = HEIGHT * .8f,
                       HPOINT9 = HEIGHT * .9f;
inline constexpr float WPOINT = WIDTH * .1f, WPOINT2 = WIDTH * .2f, WPOINT3 = WIDTH * .3f, WPOINT4 = WIDTH * .4f,
                       WPOINT5 = WIDTH * .5f, WPOINT6 = WIDTH * .6f, WPOINT7 = WIDTH * .7f, WPOINT8 = WIDTH * .8f,
                       WPOINT9 = WIDTH * .9f;

enum class GameState { PLAYING, GAME_OVER, PAUSED, HOME_MENU };
enum class HomeMenu { START, MULTI, TOGGLE, LEAVE };
enum class PauseMenu { PLAY, REDO, QUIT };
enum class GameOverMenu { REDO, QUIT };
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
    PVec2 next() const {
        PVec2 n = body[head] + dir;
        if (n.x < 0) n.x = COLS - 1;
        else if (n.x >= COLS) n.x = 0;
        else if (n.y < 0) n.y = ROWS - 1;
        else if (n.y >= ROWS) n.y = 0;
        return n;
    }
};

struct GameData {
    TileType board[TOTAL_TILES] { TileType::FREE };
    GameState gameState = GameState::HOME_MENU;
    PauseMenu pauseMenu = PauseMenu::PLAY;
    HomeMenu homeMenu = HomeMenu::START;
    GameOverMenu gameOverMenu = GameOverMenu::REDO;
    PVec2 food;
    int level = 1;
    int score = 0;
    int eaten = 0;
    bool isDead(PVec2 p) const { return board[p.index()] == TileType::SNAKE; }
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

struct UserConfig {
    wchar_t username[32];
    UINT highScore = 0U;
};

#endif