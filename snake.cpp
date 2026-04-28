#include <intrin.h>
#include <windows.h>

#include <cstdint>

#include "Snakedefs.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static Moption pauseSelection;
static Snake snake;
static GameData gd;

struct Xorshift64 {
  uint64_t state;
  Xorshift64() {
    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    state = __rdtsc() ^ (uint64_t)qpc.QuadPart;
  }
  uint64_t next() {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
  }
} static rng;

void QueueDirection(WPARAM key) {
  switch (key) {
    case VK_DOWN:
    case 'S':
      if (snake.dir.y == 0) snake.pdir = {0, 1};
      break;
    case VK_UP:
    case 'W':
      if (snake.dir.y == 0) snake.pdir = {0, -1};
      break;
    case VK_LEFT:
    case 'A':
      if (snake.dir.x == 0) snake.pdir = {-1, 0};
      break;
    case VK_RIGHT:
    case 'D':
      if (snake.dir.x == 0) snake.pdir = {1, 0};
      break;
  }
}

PVec2 GetNewFoodSpot() {
  PVec2 res{};
  do {
    uint64_t val = rng.next();
    res.x = (int)(val % COLS);
    res.y = (int)((val >> 32) % ROWS);
  } while (gd.board[res.index()] != Gtile::FREE || res == gd.food);
  return res;
}

void InitGame() {
  for (auto &t : gd.board) t = Gtile::FREE;
  snake.dir = {1, 0};
  snake.pdir = {1, 0};
  snake.head = 0;
  snake.len = 1;
  snake.body[0] = {START_X, START_Y};
  gd.board[START_INDEX] = Gtile::SNAKE;
  gd.food = GetNewFoodSpot();
  gd.gState = Gstate::PLAYING;
}

void UpdateGame() {
  snake.dir = snake.pdir;
  PVec2 nextTile = snake.body[snake.head] + snake.dir;
  if ((nextTile.x < 0 || nextTile.x >= COLS) || (nextTile.y < 0 || nextTile.y >= ROWS) || gd.board[nextTile.index()] == Gtile::SNAKE) {
    gd.gState = Gstate::GAME_OVER;
    return;
  }
  if (nextTile == gd.food) {
    snake.len++;
    snake.head = (snake.head + 1) % TOTAL_TILES;
    snake.body[snake.head] = nextTile;
    gd.food = GetNewFoodSpot();
    gd.board[nextTile.index()] = Gtile::SNAKE;
  } else {
    int tail = (snake.head - snake.len + 1 + TOTAL_TILES) % TOTAL_TILES;
    gd.board[snake.body[tail].index()] = Gtile::FREE;
    snake.head = (snake.head + 1) % TOTAL_TILES;
    snake.body[snake.head] = nextTile;
    gd.board[nextTile.index()] = Gtile::SNAKE;
  }
}

void HandleMenu(WPARAM key) {
  auto cycleOption = [](Moption opt, int delta) -> Moption { return static_cast<Moption>((static_cast<int>(opt) + delta + 3) % 3); };
  switch (key) {
    case VK_DOWN:
    case VK_RIGHT:
    case 'S':
    case 'D': pauseSelection = cycleOption(pauseSelection, 1); break;
    case VK_UP:
    case VK_LEFT:
    case 'W':
    case 'A': pauseSelection = cycleOption(pauseSelection, -1); break;
    case VK_RETURN:
    case VK_SPACE: {
      if ((pauseSelection == Moption::CONTINUE && gd.gState == Gstate::GAME_OVER) || pauseSelection == Moption::RESTART) InitGame();
      else if (pauseSelection == Moption::QUIT) PostQuitMessage(0);
      else gd.gState = Gstate::PLAYING;
      break;
    }
    case VK_ESCAPE:
    case 'P':
      if (gd.gState == Gstate::GAME_OVER) InitGame();
      gd.gState = Gstate::PLAYING;
      break;
  }
}
void PaintGame(HDC hdc, const PAINTSTRUCT &ps) {
  auto tileRect = [](PVec2 v) -> RECT {
    int px = v.x * TILESIZE, py = v.y * TILESIZE;
    return {px, py, px + TILESIZE, py + TILESIZE};
  };
  FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
  FrameRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
  for (int i = 0; i < snake.len; ++i) {
    int idx = (snake.head - i + TOTAL_TILES) % TOTAL_TILES;
    RECT snakeRect = tileRect(snake.body[idx]);
    if (i == 0) {
      FillRect(hdc, &snakeRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    } else {
      FillRect(hdc, &snakeRect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
    }
    FrameRect(hdc, &snakeRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
  }
  HBRUSH foodBrush;
  if (snake.len % 32 < 8) foodBrush = CreateSolidBrush(RGB(0, 255, 150));
  else if (snake.len % 32 < 16) foodBrush = CreateSolidBrush(RGB(0, 255, 255));
  else foodBrush = CreateSolidBrush(RGB(152, 50, 255));
  RECT foodRect = tileRect(gd.food);
  FillRect(hdc, &foodRect, foodBrush);
  DeleteObject(foodBrush);
}

void PaintPauseMenu(HDC hdc, const PAINTSTRUCT &ps) {
  RECT outerRect = {WIDTH / 4, HEIGHT / 4, WIDTH * 3 / 4, HEIGHT * 3 / 4};
  RECT titleRect = {WIDTH / 4, HEIGHT / 4, WIDTH * 3 / 4, HEIGHT * 3 / 8};
  RECT optRect0 = {WIDTH / 4, HEIGHT * 3 / 8, WIDTH * 3 / 4, HEIGHT / 2};
  RECT optRect1 = {WIDTH / 4, HEIGHT / 2, WIDTH * 3 / 4, HEIGHT * 5 / 8};
  RECT optRect2 = {WIDTH / 4, HEIGHT * 5 / 8, WIDTH * 3 / 4, HEIGHT * 3 / 4};
  auto makeFont = [](int sz, int wt) -> HFONT {
    return CreateFont(sz, 0, 0, 0, wt, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                      DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
  };
  HFONT titleFont = makeFont(-36, 500);
  HFONT optFont = makeFont(-24, 400);
  FillRect(hdc, &outerRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
  FrameRect(hdc, &outerRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
  HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);
  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, RGB(255, 255, 255));
  DrawText(hdc, L"PAUSED", -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  SelectObject(hdc, optFont);
  auto drawOption = [&](Moption opt, RECT &rect, const wchar_t *label) {
    if (pauseSelection == opt) {
      FillRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
      SetTextColor(hdc, RGB(0, 0, 0));
    } else SetTextColor(hdc, RGB(255, 255, 255));
    DrawText(hdc, label, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  };
  drawOption(Moption::CONTINUE, optRect0, L"CONTINUE");
  drawOption(Moption::RESTART, optRect1, L"RESTART");
  drawOption(Moption::QUIT, optRect2, L"QUIT");
  SelectObject(hdc, oldFont);
  DeleteObject(titleFont);
  DeleteObject(optFont);
}
HWND WindowInit(HINSTANCE hInstance) {
  WNDCLASS wc{};
  DWORD WindowStyles = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
  const wchar_t CLASS_NAME[] = L"Meo Win32 Snake", WINDOW_NAME[] = L"MeoSnake";
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  RegisterClass(&wc);
  RECT rc = {0, 0, WIDTH, HEIGHT};
  AdjustWindowRectEx(&rc, WindowStyles, FALSE, 0);
  HWND hwnd = CreateWindowEx(0, CLASS_NAME, WINDOW_NAME, WindowStyles, CW_USEDEFAULT, CW_USEDEFAULT, (rc.right - rc.left),
                             (rc.bottom - rc.top), NULL, NULL, hInstance, NULL);
  ShowWindow(hwnd, SW_SHOW);
  InitGame();
  UpdateWindow(hwnd);
  SetTimer(hwnd, 0, 150, NULL);

  return hwnd;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
  HWND hwnd = WindowInit(hInstance);
  if (hwnd == NULL) return 0;
  MSG msg{};
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_KEYDOWN: {
      switch (gd.gState) {
        case Gstate::GAME_OVER:
        case Gstate::PAUSED:
          HandleMenu(wParam);
          InvalidateRect(hwnd, NULL, FALSE);
          break;
        case Gstate::PLAYING:
          if (wParam == VK_ESCAPE || wParam == 'P') {
            gd.gState = Gstate::PAUSED;
            InvalidateRect(hwnd, NULL, FALSE);
          } else QueueDirection(wParam);
          break;
      }
      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      PaintGame(hdc, ps);
      if (gd.gState != Gstate::PLAYING) PaintPauseMenu(hdc, ps);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_TIMER: {
      if (gd.gState == Gstate::PLAYING) {
        UpdateGame();
        InvalidateRect(hwnd, NULL, FALSE);
      }
      return 0;
    }
    case WM_DESTROY: PostQuitMessage(0); return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}