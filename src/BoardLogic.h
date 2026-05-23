#ifndef SNAKE_H
#define SNAKE_H

#include <intrin.h>
#include <windows.h>

#include "Constants.h"
#include "Rendering.h"

enum class MenuNames { PLAYING, HOME, GAME_OVER, PAUSE };
enum class GameOverMenu { REDO, QUIT };
enum class PauseMenu { PLAY, REDO, QUIT };
enum class HomeMenu { START, MULTI, TOGGLE, LEAVE };
enum class TileType { FREE, SNAKE };

struct Menus {
    MenuNames screen = MenuNames::PLAYING;
    GameOverMenu go_menu = GameOverMenu::REDO;
    PauseMenu p_menu = PauseMenu::PLAY;
    HomeMenu h_menu = HomeMenu::START;
};

struct PVec2 {
    int x;
    int y;
    PVec2 operator+(const PVec2 &other) const { return { x + other.x, y + other.y }; }
    bool operator==(const PVec2 &other) const { return x == other.x && y == other.y; }
    int index() const { return y * COLS + x; }
};

struct Xorshift64 {
    UINT64 state;
    Xorshift64() {
        LARGE_INTEGER qpc;
        QueryPerformanceCounter(&qpc);
        state = __rdtsc() ^ (UINT64)qpc.QuadPart;
    }
    UINT64 next() {
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
    void lvlUp() {
        len = 1;
        body[0] = body[head];
        head = 0;
    }
};

class GameData {
   public:
    TileType board[TOTAL_TILES] { TileType::FREE };
    Menus *gS = new Menus();
    PVec2 food;
    Xorshift64 seed = Xorshift64();
    int level {};
    int score {};
    int eaten {};
    void getNewFoodSpot() {
        PVec2 res;
        do {
            UINT64 val = seed.next();
            res.x = static_cast<int>(val % COLS);
            res.y = static_cast<int>((val >> 32) % ROWS);
        } while (board[res.index()] == TileType::SNAKE || res == food);
        food = res;
    }
    void resetMenus(MenuNames screen) {
        gS->screen = screen;
        gS->go_menu = GameOverMenu::REDO;
        gS->p_menu = PauseMenu::PLAY;
        gS->h_menu = HomeMenu::START;
    }
    void lvlUp(int i) {
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