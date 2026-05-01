#include "render.h"
#include <cstdint>
#include <intrin.h>

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

PVec2 GetNewFoodSpot() {
  PVec2 res {};
  do {
    uint64_t val = rng.next();
    res.x = static_cast<int>(val % COLS);
    res.y = static_cast<int>((val >> 32) % ROWS);
  } while (gd.board[res.index()] != Gtile::FREE || res == gd.food);
  return res;
}

void InitGame() {
  gd.cleanTiles();
  snake.dir = {1, 0};
  snake.pDir = {1, 0};
  snake.head = 0;
  snake.len = 1;
  snake.body[0] = {START_X, START_Y};
  gd.board[START_INDEX] = Gtile::SNAKE;
  gd.food = GetNewFoodSpot();
  gd.gState = Gstate::PLAYING;
}

void UpdateGame() {
  snake.setDir();
  PVec2 nextTile = snake.next();
  if (gd.notFree(nextTile)) gd.gState = Gstate::GAME_OVER;
  else {
    gd.board[nextTile.index()] = Gtile::SNAKE;
    snake.head = (snake.head + 1) % TOTAL_TILES;
    snake.body[snake.head] = nextTile;
    if (nextTile == gd.food) {
      gd.food = GetNewFoodSpot();
      snake.len++;
    } else
      gd.board[snake.body[(snake.head - snake.len + TOTAL_TILES) % TOTAL_TILES].index()] =
          Gtile::FREE;
  }
}

void HandleMenu(WPARAM key) {
  auto cycleOption = [](Moption opt, int delta) -> Moption {
    return static_cast<Moption>((static_cast<int>(opt) + delta + 3) % 3);
  };
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
      if ((pauseSelection == Moption::CONTINUE && gd.gState == Gstate::GAME_OVER) ||
          pauseSelection == Moption::RESTART)
        InitGame();
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
    case WM_KEYDOWN: {
      switch (gd.gState) {
        case Gstate::PLAYING:
          if (wParam == VK_ESCAPE || wParam == 'P') {
            InvalidateRect(hwnd, NULL, FALSE);
            gd.gState = Gstate::PAUSED;
          } else QueueDirection(wParam);
          break;
        default:
          HandleMenu(wParam);
          InvalidateRect(hwnd, NULL, FALSE);
          break;
      }
      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      RenderContext ctx {hdc, ps};
      PaintGame(ctx, snake, gd);
      if (gd.gState != Gstate::PLAYING) PaintMenu(ctx, pauseSelection);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_TIMER:
      if (gd.gState == Gstate::PLAYING) {
        UpdateGame();
        InvalidateRect(hwnd, NULL, FALSE);
      }
      return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}