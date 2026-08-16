#ifndef SNAKE_H
#define SNAKE_H

#include <intrin.h>
#include <windows.h>

#include <cstdint>

#include "Rendering.h"

using std::uint64_t, std::int32_t, std::uint32_t, std::uint8_t;

inline constexpr uint16_t COLS = WIDTH / TILESIZE;
inline constexpr uint16_t ROWS = HEIGHT / TILESIZE;
inline constexpr uint32_t TOTAL_TILES = ROWS * COLS;
inline constexpr uint16_t START_X = COLS / 2;
inline constexpr uint16_t START_Y = ROWS / 2;
inline constexpr uint32_t START_INDEX = START_Y * COLS + START_X;

enum class MenuNames : uint8_t { PLAYING, HOME, GAME_OVER, PAUSE };
enum class GameOverMenu : uint8_t { REDO, QUIT };
enum class PauseMenu : uint8_t { PLAY, REDO, QUIT };
enum class HomeMenu : uint8_t { START, MULTI, TOGGLE, LEAVE };
enum class TileType : uint8_t { FREE, SNAKE };

struct Menus {
    MenuNames screen {};
    GameOverMenu go_menu {};
    PauseMenu p_menu {};
    HomeMenu h_menu {};
};

struct PVec2 {
    int32_t x;
    int32_t y;
    PVec2 operator+(const PVec2 &other) const { return { x + other.x, y + other.y }; }
    bool operator==(const PVec2 &other) const { return x == other.x && y == other.y; }
    uint32_t index() const { return y * static_cast<int32_t>(COLS) + x; }
};

struct Xorshift64 {
    uint64_t state;
    Xorshift64() {
        LARGE_INTEGER qpc;
        QueryPerformanceCounter(&qpc);
        state = __rdtsc() ^ static_cast<uint64_t>(qpc.QuadPart);
    }
    uint64_t next() {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return state;
    }
};

struct Snake {
    PVec2 dir = { 1, 0 };
    PVec2 pDir = { 1, 0 };
    PVec2 body[TOTAL_TILES] = { { START_X, START_Y } };
    uint32_t head = 0;
    uint32_t len = 1;
    PVec2 next() const {
        auto n = body[head] + dir;
        if (n.x < 0) n.x = static_cast<int32_t>(COLS) - 1;
        else if (n.x >= static_cast<int32_t>(COLS)) n.x = 0;
        else if (n.y < 0) n.y = static_cast<int32_t>(ROWS) - 1;
        else if (n.y >= static_cast<int32_t>(ROWS)) n.y = 0;
        return n;
    }
    void lvlUp() {
        len = 1;
        body[0] = body[head];
        head = 0;
    }
};

class GameData {
   public:
    TileType board[TOTAL_TILES] {};
    Menus gS {};
    PVec2 food;
    Xorshift64 seed = Xorshift64();
    uint32_t score {};
    uint16_t level {};
    uint16_t eaten {};
    void getNewFoodSpot() {
        PVec2 res;
        do {
            auto val { seed.next() };
            res.x = static_cast<int32_t>(val % COLS);
            res.y = static_cast<int32_t>((val >> 32) % ROWS);
        } while (board[res.index()] == TileType::SNAKE || res == food);
        food = res;
    }
    void resetMenus(MenuNames screen) {
        gS.screen = screen;
        gS.go_menu = GameOverMenu::REDO;
        gS.p_menu = PauseMenu::PLAY;
        gS.h_menu = HomeMenu::START;
    }
    void lvlUp(uint32_t i) {
        wipeBoard();
        board[i] = TileType::SNAKE;
        getNewFoodSpot();
    }
    void reset(Snake &snake) {
        wipeBoard();
        resetMenus(MenuNames::PLAYING);
        level = 1;
        eaten = 0;
        score = 0;
        getNewFoodSpot();
        snake = Snake();
    }
    void pause() { resetMenus(MenuNames::PAUSE); }
    void resume() { resetMenus(MenuNames::PLAYING); }

   private:
    void wipeBoard() {
        for (auto &t : board) t = TileType::FREE;
    }
};

void InitRenderContext(RenderContext &ctx, HWND hwnd);

void PaintGame(RenderContext &ctx, const Snake &snake, const GameData &gd);

#endif
