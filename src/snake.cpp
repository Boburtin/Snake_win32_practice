#include <intrin.h>

#include <cstdint>

#include "render.h"

static Snake snake;
static GameData gd;
static RenderContext ctx;
constexpr UINT_PTR TIMER_ID = 67ULL;
static UINT GAME_TICK = 100U;

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
} static rng;

PVec2 GetNewFoodSpot() {
    PVec2 res {};
    do {
        UINT64 val = rng.next();
        res.x = static_cast<int>(val % COLS);
        res.y = static_cast<int>((val >> 32) % ROWS);
    } while (gd.board[res.index()] != TileType::FREE || res == gd.food);
    return res;
}

void QueueDirection(WPARAM key) {
    switch (key) {
        case VK_DOWN:
        case 'S':
            if (snake.dir.y == 0) snake.pDir = { 0, 1 };
            break;
        case VK_UP:
        case 'W':
            if (snake.dir.y == 0) snake.pDir = { 0, -1 };
            break;
        case VK_LEFT:
        case 'A':
            if (snake.dir.x == 0) snake.pDir = { -1, 0 };
            break;
        case VK_RIGHT:
        case 'D':
            if (snake.dir.x == 0) snake.pDir = { 1, 0 };
            break;
    }
}

void UpdateGame(HWND hwnd) {
    snake.dir = snake.pDir;
    PVec2 next = snake.next();
    if (gd.isDead(next)) {
        gd.gameState = GameState::GAME_OVER;
        return;
    }
    gd.board[next.index()] = TileType::SNAKE;
    snake.head = (snake.head + 1) % TOTAL_TILES;
    snake.body[snake.head] = next;
    if (next != gd.food) {
        int tail_index = snake.body[(snake.head - snake.len + TOTAL_TILES) % TOTAL_TILES].index();
        gd.board[tail_index] = TileType::FREE;
        return;
    }
    if (snake.len > gd.level * 10 || (gd.level > 10 && snake.len > 100)) {
        gd.score += gd.level <= 10 ? gd.level : 10;
        gd.level++;
        gd.reset(snake.body[snake.head].index(), GetNewFoodSpot, snake);
        KillTimer(hwnd, TIMER_ID);
        GAME_TICK = GAME_TICK >= 80 ? GAME_TICK - 10 : GAME_TICK >= 50 ? GAME_TICK - 5 : GAME_TICK;
        SetTimer(hwnd, TIMER_ID, GAME_TICK, NULL);
        ctx.fullRedraw = TRUE;
    } else {
        snake.len++;
        gd.score++;
        gd.food = GetNewFoodSpot();
        ctx.foodEaten = TRUE;
    }
}

template <typename T>
void cycleMenu(T &option, int dir, T first, T last) {
    int val = static_cast<int>(option) + dir;
    int min = static_cast<int>(first);
    int max = static_cast<int>(last);
    if (val < min) val = max;
    if (val > max) val = min;
    option = static_cast<T>(val);
}

void HandleMenu(WPARAM key) {
    switch (key) {
        case VK_DOWN:
        case VK_RIGHT:
        case 'S':
        case 'D':
            if (gd.gameState == GameState::PAUSED) cycleMenu(gd.pauseMenu, 1, PauseMenu::RESUME, PauseMenu::QUIT);
            else cycleMenu(gd.homeMenu, 1, HomeMenu::PLAY, HomeMenu::QUIT);
            break;
        case VK_UP:
        case VK_LEFT:
        case 'W':
        case 'A':
            if (gd.gameState == GameState::PAUSED) cycleMenu(gd.pauseMenu, -1, PauseMenu::RESUME, PauseMenu::QUIT);
            else cycleMenu(gd.homeMenu, -1, HomeMenu::PLAY, HomeMenu::QUIT);
            break;
        case VK_ESCAPE:
        case VK_RETURN:
        case VK_SPACE:
            if (gd.gameState == GameState::PAUSED) {
                switch (gd.pauseMenu) {
                    case PauseMenu::RESUME:
                        gd.resume();
                        ctx.fullRedraw = TRUE;
                        break;
                    case PauseMenu::RESTART:
                        gd.reset(snake, GetNewFoodSpot);
                        ctx.fullRedraw = TRUE;
                        break;
                    case PauseMenu::BACK: gd.gameState = GameState::HOME_MENU; break;
                    case PauseMenu::QUIT: PostQuitMessage(0); break;
                }
            } else if (gd.gameState == GameState::GAME_OVER || gd.gameState == GameState::HOME_MENU) {
                switch (gd.homeMenu) {
                    case HomeMenu::PLAY:
                        gd.reset(snake, GetNewFoodSpot);
                        ctx.fullRedraw = TRUE;
                        break;
                    case HomeMenu::MULTI: break;
                    case HomeMenu::TOGGLE: break;
                    case HomeMenu::QUIT: PostQuitMessage(0); break;
                }
            }
    }
}

HWND WindowInit(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASS wc {};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"3meo snake";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInstance;
    RegisterClass(&wc);

    RECT rc = { 0, 0, WIDTH, HEIGHT + 2 * TILESIZE };
    DWORD WindowStyles = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

    AdjustWindowRectEx(&rc, WindowStyles, FALSE, 0);
    HWND hwnd = CreateWindowEx(0, L"3meo snake", L"Snake", WindowStyles, CW_USEDEFAULT, CW_USEDEFAULT,
                               rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    InitRenderContext(ctx, hwnd);
    gd.board[START_INDEX] = TileType::SNAKE;
    gd.food = GetNewFoodSpot();
    UpdateWindow(hwnd);
    SetTimer(hwnd, TIMER_ID, GAME_TICK, NULL);
    return hwnd;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    HWND hwnd = WindowInit(hInstance, nCmdShow);
    if (hwnd == NULL) return 0;
    MSG msg {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            switch (gd.gameState) {
                case GameState::HOME_MENU:
                case GameState::GAME_OVER:
                case GameState::PAUSED:
                    HandleMenu(wParam);
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case GameState::PLAYING:
                    if (wParam == VK_SPACE || wParam == VK_ESCAPE || wParam == VK_RETURN)
                        gd.pause(), InvalidateRect(hwnd, NULL, FALSE);
                    else QueueDirection(wParam);
                    break;
            }
            return 0;
        case WM_PAINT: {
            ctx.pRT->BeginDraw();
            PaintGame(ctx, snake, gd);
            ctx.fullRedraw = FALSE;
            if (gd.gameState == GameState::GAME_OVER || gd.gameState == GameState::HOME_MENU) PaintHomeMenu(ctx, gd);
            else if (gd.gameState == GameState::PAUSED) PaintPauseMenu(ctx, gd);
            ctx.pRT->EndDraw();
            ValidateRect(hwnd, NULL);
            return 0;
        }
        case WM_TIMER:
            if (gd.gameState == GameState::PLAYING) {
                UpdateGame(hwnd);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        case WM_DESTROY:
            ctx.release();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}