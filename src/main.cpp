#include <windows.h>

#include "BoardLogic.h"

static Snake snake {};
static GameData gd {};
static RenderContext ctx {};
constexpr UINT_PTR TIMER_ID = 67ULL;
constexpr UINT BASELINE_TICK_SPEED = 120U;

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
    if (gd.board[next.index()] == TileType::SNAKE) {
        gd.resetMenus(MenuNames::GAME_OVER);
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
    if (snake.len > gd.level * 10) {
        gd.level++;
        snake.lvlUp();
        gd.lvlUp(snake.body[0].index());
        SetTimer(hwnd, TIMER_ID, gd.level < 5 ? BASELINE_TICK_SPEED - gd.level * 10 : 70, NULL);
        ctx.fullRedraw = TRUE;
    } else {
        snake.len++;
        gd.getNewFoodSpot();
        ctx.foodEaten = TRUE;
    }
    gd.eaten++;
    gd.score += gd.level * 10;
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

void HandleMenus(WPARAM key) {
    switch (key) {
        case VK_DOWN:
        case VK_RIGHT:
        case 'S':
        case 'D':
            if (gd.gS->screen == MenuNames::PAUSE) cycleMenu(gd.gS->p_menu, 1, PauseMenu::PLAY, PauseMenu::QUIT);
            if (gd.gS->screen == MenuNames::GAME_OVER)
                cycleMenu(gd.gS->go_menu, 1, GameOverMenu::REDO, GameOverMenu::QUIT);
            if (gd.gS->screen == MenuNames::HOME) cycleMenu(gd.gS->h_menu, 1, HomeMenu::START, HomeMenu::LEAVE);
            break;
        case VK_UP:
        case VK_LEFT:
        case 'W':
        case 'A':
            if (gd.gS->screen == MenuNames::PAUSE) cycleMenu(gd.gS->p_menu, -1, PauseMenu::PLAY, PauseMenu::QUIT);
            if (gd.gS->screen == MenuNames::GAME_OVER)
                cycleMenu(gd.gS->go_menu, -1, GameOverMenu::REDO, GameOverMenu::QUIT);
            if (gd.gS->screen == MenuNames::HOME) cycleMenu(gd.gS->h_menu, -1, HomeMenu::START, HomeMenu::LEAVE);
            break;
        case VK_ESCAPE:
        case VK_RETURN:
        case VK_SPACE:
            if (gd.gS->screen == MenuNames::PAUSE) {
                switch (gd.gS->p_menu) {
                    case PauseMenu::PLAY:
                        gd.resetMenus(MenuNames::PLAYING);
                        ctx.fullRedraw = TRUE;
                        break;
                    case PauseMenu::REDO:
                        gd.reset(snake);
                        ctx.fullRedraw = TRUE;
                        break;
                    case PauseMenu::QUIT: gd.gS->screen = MenuNames::HOME; break;
                }
            } else if (gd.gS->screen == MenuNames::GAME_OVER) {
                switch (gd.gS->go_menu) {
                    case GameOverMenu::REDO:
                        gd.reset(snake);
                        ctx.fullRedraw = TRUE;
                        break;
                    case GameOverMenu::QUIT: gd.gS->screen = MenuNames::HOME; break;
                }
            }

            else if (gd.gS->screen == MenuNames::HOME) {
                switch (gd.gS->h_menu) {
                    case HomeMenu::START:
                        gd.reset(snake);
                        ctx.fullRedraw = TRUE;
                        break;
                    case HomeMenu::MULTI: break;
                    case HomeMenu::TOGGLE: break;
                    case HomeMenu::LEAVE: PostQuitMessage(0); break;
                }
            }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            switch (gd.gS->screen) {
                case MenuNames::HOME:
                case MenuNames::GAME_OVER:
                case MenuNames::PAUSE:
                    HandleMenus(wParam);
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case MenuNames::PLAYING:
                    if (wParam == VK_SPACE || wParam == VK_ESCAPE || wParam == VK_RETURN) {
                        ctx.fullRedraw = TRUE;
                        gd.resetMenus(MenuNames::PAUSE);
                        InvalidateRect(hwnd, NULL, FALSE);
                    } else QueueDirection(wParam);
                    break;
            }
            return 0;
        case WM_PAINT: {
            ctx.pRT->BeginDraw();
            if (gd.gS->screen == MenuNames::HOME) PaintMenu(ctx, homeMenu, gd.gS->h_menu);
            if (gd.gS->screen == MenuNames::GAME_OVER) PaintMenu(ctx, gameOverMenu, gd.gS->go_menu);
            if (gd.gS->screen == MenuNames::PAUSE) PaintMenu(ctx, pauseMenu, gd.gS->p_menu);
            if (gd.gS->screen == MenuNames::PLAYING) PaintGame(ctx, snake, gd);
            ctx.fullRedraw = FALSE;
            ctx.pRT->EndDraw();
            ValidateRect(hwnd, NULL);
            return 0;
        }
        case WM_TIMER:
            if (gd.gS->screen == MenuNames::PLAYING) {
                UpdateGame(hwnd);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        case WM_DESTROY:
            ctx.release();
            delete gd.gS;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static HWND WindowInit(HINSTANCE hInstance, int nCmdShow) {
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
    gd.resetMenus(MenuNames::HOME);
    gd.lvlUp(START_INDEX);
    UpdateWindow(hwnd);
    SetTimer(hwnd, TIMER_ID, BASELINE_TICK_SPEED, NULL);
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
