#ifndef RENDER_H
#define RENDER_H

#define WIN32_LEAN_AND_MEAN
#include <d2d1.h>
#include <dwrite.h>
#include <intrin.h>
#include <windows.h>

#include <cmath>
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

enum class MenuNames { PLAYING, HOME, GAME_OVER, PAUSE };
enum class GameOverMenu { REDO, QUIT };
enum class PauseMenu { PLAY, REDO, QUIT };
enum class HomeMenu { START, MULTI, TOGGLE, LEAVE };
struct Menus {
    MenuNames screen = MenuNames::PLAYING;
    GameOverMenu go_menu = GameOverMenu::REDO;
    PauseMenu p_menu = PauseMenu::PLAY;
    HomeMenu h_menu = HomeMenu::START;
};

struct MenuOption {
    const D2D1_RECT_F rect;
    const wchar_t *label;
    const bool locked = false;
};

struct MenuRect {
    const D2D1_RECT_F frame;
    const D2D1_RECT_F header;
    const wchar_t *title;
    const MenuOption options[4];
    const int count;
};

struct LevelTheme {
    D2D1_COLOR_F head;
    D2D1_COLOR_F body;
    D2D1_COLOR_F food;
};

struct HudRects {
    D2D1_RECT_F frame = { 0.f, HEIGHT * 1.f, WIDTH * 1.f, HEIGHT + TILESIZE * 2.f };
    D2D1_RECT_F score = { 0.f, HEIGHT * 1.f, WIDTH * .5f, HEIGHT + TILESIZE * 2.f };
    D2D1_RECT_F level = { WIDTH * .5f, HEIGHT * 1.f, WIDTH * 1.f, HEIGHT + TILESIZE * 2.f };
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

struct RenderContext {
    ID2D1Factory *pFactory;
    ID2D1HwndRenderTarget *pRT;
    IDWriteFactory *pDWFactory;
    ID2D1SolidColorBrush *pBrush;
    IDWriteTextFormat *pHudFont;
    IDWriteTextFormat *pHeaderFont;
    IDWriteTextFormat *pLockedOptionFont;
    BOOL fullRedraw = TRUE;
    BOOL foodEaten = FALSE;
    void release() {
        pLockedOptionFont->Release();
        pHeaderFont->Release();
        pHudFont->Release();
        pBrush->Release();
        pDWFactory->Release();
        pRT->Release();
        pFactory->Release();
    }
};

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
    void lvlUp() {
        len = 1;
        body[0] = body[head];
        head = 0;
    }
};

void drawMenuSection(RenderContext &ctx, int currentSelection, int thisIndex, const MenuRect &mr);

template <typename T>
inline void PaintMenu(RenderContext &ctx, const MenuRect &mr, T currentSelection) {
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(mr.frame, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    ctx.pRT->DrawRectangle(mr.frame, ctx.pBrush);
    ctx.pRT->DrawTextW(mr.title, wcslen(mr.title), ctx.pHeaderFont, mr.header, ctx.pBrush);
    for (int i {}; i < mr.count; ++i) { drawMenuSection(ctx, (int)currentSelection, i, mr); }
}
enum class TileType { FREE, SNAKE };

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

HWND WindowInit(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

inline const MenuRect gameOverMenu { { WPOINT2, HPOINT2, WPOINT8, HPOINT5 },
                                     { WPOINT2, HPOINT2, WPOINT8, HPOINT3 },
                                     L"* Game Over *",
                                     { { { WPOINT2, HPOINT3, WPOINT8, HPOINT4 }, L"redo", false },
                                       { { WPOINT2, HPOINT4, WPOINT8, HPOINT5 }, L"quit", false } },
                                     2 };

inline const MenuRect homeMenu { { WPOINT2, HPOINT2, WPOINT8, HPOINT7 },
                                 { WPOINT2, HPOINT2, WPOINT8, HPOINT3 },
                                 L"* Home Menu *",
                                 { { { WPOINT2, HPOINT3, WPOINT8, HPOINT4 }, L"start", false },
                                   { { WPOINT2, HPOINT4, WPOINT8, HPOINT5 }, L"multi", true },
                                   { { WPOINT2, HPOINT5, WPOINT8, HPOINT6 }, L"toggle ", true },
                                   { { WPOINT2, HPOINT6, WPOINT8, HPOINT7 }, L"leave", false } },
                                 4 };

inline const MenuRect pauseMenu { { WPOINT2, HPOINT2, WPOINT8, HPOINT6 },
                                  { WPOINT2, HPOINT2, WPOINT8, HPOINT3 },
                                  L"* Paused *",
                                  { { { WPOINT2, HPOINT3, WPOINT8, HPOINT4 }, L"play", false },
                                    { { WPOINT2, HPOINT4, WPOINT8, HPOINT5 }, L"redo", false },
                                    { { WPOINT2, HPOINT5, WPOINT8, HPOINT6 }, L"quit   ", false } },
                                  3 };

inline LevelTheme levelTheme(int level) {
    auto s = [level](float base) { return fmodf(base + .1f * level, 1.f); };
    return { { s(.75f), .75f, 0.f, 1.f }, { 1.f, 1.f, 1.f, .75f }, { s(.1f), 1.f, s(0.f), 1.f } };
}

void InitRenderContext(RenderContext &ctx, HWND hwnd);

void PaintGame(RenderContext &ctx, const Snake &snake, const GameData &gd);

#endif