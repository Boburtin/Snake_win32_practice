#include <intrin.h>

#include <cstdint>

#include "render.h"

static Snake snake;
static GameData gd;
static RenderContext ctx;

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
    } while (gd.board[res.index()] != T_Type::FREE || res == gd.food);
    return res;
}

void QueueDirection(WPARAM key) {
    switch (key) {
        case VK_DOWN:
        case 'S':
            if (snake.dir.y == 0) snake.pDir = {0, 1};
            break;
        case VK_UP:
        case 'W':
            if (snake.dir.y == 0) snake.pDir = {0, -1};
            break;
        case VK_LEFT:
        case 'A':
            if (snake.dir.x == 0) snake.pDir = {-1, 0};
            break;
        case VK_RIGHT:
        case 'D':
            if (snake.dir.x == 0) snake.pDir = {1, 0};
            break;
    }
}

void UpdateGame() {
    snake.dir = snake.pDir;
    PVec2 nextTile = snake.next();
    if (gd.gonnaDie(nextTile)) gd.gState = G_Status::GAME_OVER;
    else {
        gd.board[nextTile.index()] = T_Type::SNAKE;
        snake.head = (snake.head + 1) % TOTAL_TILES;
        snake.body[snake.head] = nextTile;
        if (nextTile == gd.food) {
            gd.food = GetNewFoodSpot();
            snake.len++;
        } else gd.board[snake.body[(snake.head - snake.len + TOTAL_TILES) % TOTAL_TILES].index()] = T_Type::FREE;
    }
}

template <typename T> void cycleMenu(T &option, int dir, T first, T last) {
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
            if (gd.gState == G_Status::PAUSED) cycleMenu(gd.mOption, 1, M_Paused::RESUME, M_Paused::QUIT);
            else cycleMenu(gd.mHome, 1, M_Home::SOLO, M_Home::EXIT);
            break;
        case VK_UP:
        case VK_LEFT:
        case 'W':
        case 'A':
            if (gd.gState == G_Status::PAUSED) cycleMenu(gd.mOption, -1, M_Paused::RESUME, M_Paused::QUIT);
            else cycleMenu(gd.mHome, -1, M_Home::SOLO, M_Home::EXIT);
            break;
        case VK_ESCAPE:
        case VK_RETURN:
        case VK_SPACE: {
            if (gd.gState == G_Status::PAUSED) {
                switch (gd.mOption) {
                    case M_Paused::RESUME: gd.cont(); break;
                    case M_Paused::RESTART: gd.reset(snake, GetNewFoodSpot); break;
                    case M_Paused::QUIT: PostQuitMessage(0); break;
                }
                return;
            }
            switch (gd.mHome) {
                case M_Home::SOLO: gd.reset(snake, GetNewFoodSpot); break;
                case M_Home::MULTI:
                case M_Home::SCORES:
                case M_Home::SETTINGS:
                case M_Home::EXIT: PostQuitMessage(0); break;
            }
        }
    }
}

HWND WindowInit(HINSTANCE hInstance, int nCmdShow) {

    WNDCLASS wc {};
    HWND hwnd;
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"Meo Bad Snake";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInstance;
    RegisterClass(&wc);

    RECT rc = {0, 0, WIDTH, HEIGHT + 2 * TILESIZE};
    DWORD WindowStyles = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

    AdjustWindowRectEx(&rc, WindowStyles, FALSE, 0);
    hwnd = CreateWindowEx(0, L"Meo Bad Snake", L"Snakeo", WindowStyles, CW_USEDEFAULT, CW_USEDEFAULT,
                          rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    InitRenderContext(ctx, hwnd);
    gd.reset(snake, GetNewFoodSpot);
    UpdateWindow(hwnd);
    SetTimer(hwnd, 0, BASE_SPEED, NULL);
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
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            switch (gd.gState) {
                case G_Status::GAME_OVER:
                case G_Status::PAUSED:
                    HandleMenu(wParam);
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case G_Status::PLAYING:
                    if (wParam == VK_SPACE || wParam == VK_ESCAPE || wParam == VK_RETURN)
                        gd.pause(), InvalidateRect(hwnd, NULL, FALSE);
                    else QueueDirection(wParam);
                    break;
            }
            return 0;
        case WM_PAINT: {
            ctx.pRT->BeginDraw();
            PaintGame(ctx, snake, gd);
            if (gd.gState == G_Status::GAME_OVER) PaintHome(ctx, gd);
            else if (gd.gState == G_Status::PAUSED) PaintMenu(ctx, gd);
            ctx.pRT->EndDraw();
            ValidateRect(hwnd, NULL);
            return 0;
        }
        case WM_TIMER:
            if (gd.gState == G_Status::PLAYING) {
                UpdateGame();
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