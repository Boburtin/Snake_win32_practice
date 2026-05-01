#include "render.h"
#include <string>

void PaintGame(RenderContext &ctx, Snake &snake, GameData &gd) {
  auto tileRect = [](PVec2 v) -> RECT {
    return {v.x * TILESIZE, v.y * TILESIZE, v.x * TILESIZE + TILESIZE, v.y * TILESIZE + TILESIZE};
  };
  FillRect(ctx.hdc, &ctx.ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
  FrameRect(ctx.hdc, &ctx.ps.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
  for (int i {0}; i < snake.len; ++i) {
    RECT snakeRect = tileRect(snake.body[(snake.head + TOTAL_TILES - i) % TOTAL_TILES]);
    FillRect(ctx.hdc, &snakeRect, (HBRUSH)GetStockObject(i == 0 ? WHITE_BRUSH : LTGRAY_BRUSH));
  }
  RECT foodRect = tileRect(gd.food);
  HBRUSH foodBrush = CreateSolidBrush(snake.len % 32 < 10 ? RGB(0, 255, 0) : RGB(150, 50, 150));
  FillRect(ctx.hdc, &foodRect, foodBrush);
  RECT hudRect = {0, HEIGHT, WIDTH, HEIGHT + 2 * TILESIZE};
  // RECT leftRect = {WIDTH / 8, HEIGHT, WIDTH / 4, HEIGHT + 2 * TILESIZE};
  // RECT midRect = {WIDTH * 3 / 8, HEIGHT, WIDTH * 5 / 8, HEIGHT + 2 * TILESIZE};
  // RECT rightRect = {WIDTH * 6 / 8, HEIGHT, WIDTH * 7 / 8, HEIGHT + 2 * TILESIZE};
  HFONT hudFont = CreateFont(-TILESIZE, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
  HFONT oldFont = (HFONT)SelectObject(ctx.hdc, hudFont); 
  SetBkMode(ctx.hdc, TRANSPARENT);
  SetTextColor(ctx.hdc, RGB(255, 255, 255));
  wchar_t buf[16];
  swprintf_s(buf, L"Score: %d", snake.len);
  FillRect(ctx.hdc, &hudRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
  FrameRect(ctx.hdc, &hudRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
  DrawText(ctx.hdc, buf, -1, &hudRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  SelectObject(ctx.hdc, oldFont);
  DeleteObject(foodBrush);
  DeleteObject(hudFont);
};

void PaintMenu(RenderContext &ctx, Moption option) {
  auto drawOption = [&](Moption thisOption, RECT &rect, const wchar_t *label) {
    if (thisOption == option) FillRect(ctx.hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
    SetTextColor(ctx.hdc, thisOption == option ? RGB(0, 0, 0) : RGB(200, 200, 200));
    DrawText(ctx.hdc, label, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  };
  RECT mParent = {WIDTH / 4, HEIGHT / 4, WIDTH * 3 / 4, HEIGHT * 3 / 4};
  RECT mHeader = {WIDTH / 4, HEIGHT / 4, WIDTH * 3 / 4, HEIGHT * 3 / 8};
  RECT mOne = {WIDTH / 4, HEIGHT * 3 / 8, WIDTH * 3 / 4, HEIGHT / 2};
  RECT mTwo = {WIDTH / 4, HEIGHT / 2, WIDTH * 3 / 4, HEIGHT * 5 / 8};
  RECT mThree = {WIDTH / 4, HEIGHT * 5 / 8, WIDTH * 3 / 4, HEIGHT * 3 / 4};
  HFONT headerFont = CreateFont(-TILESIZE * 2, 0, 0, 0, 500, TRUE, FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
  HFONT optionFont = CreateFont((int)(1.5f * -TILESIZE), 0, 0, 0, 400, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
  FillRect(ctx.hdc, &mParent, (HBRUSH)GetStockObject(BLACK_BRUSH));
  FrameRect(ctx.hdc, &mParent, (HBRUSH)GetStockObject(WHITE_BRUSH));
  HFONT oldFont = (HFONT)SelectObject(ctx.hdc, headerFont);
  SetBkMode(ctx.hdc, TRANSPARENT);
  SetTextColor(ctx.hdc, RGB(100, 100, 100));
  DrawText(ctx.hdc, L"SNAKE MENU", -1, &mHeader, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  SelectObject(ctx.hdc, optionFont);
  drawOption(Moption::CONTINUE, mOne, L"CONTINUE");
  drawOption(Moption::RESTART, mTwo, L"RESTART");
  drawOption(Moption::QUIT, mThree, L"EXIT GAME");
  SelectObject(ctx.hdc, oldFont);
  DeleteObject(headerFont);
  DeleteObject(optionFont);
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
  DWORD WindowStyles = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
  AdjustWindowRectEx(&rc, WindowStyles, FALSE, 0);
  hwnd = CreateWindowEx(0, L"Meo Bad Snake", L"Snakeo", WindowStyles, CW_USEDEFAULT, CW_USEDEFAULT,
                        rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
  ShowWindow(hwnd, nCmdShow);
  InitGame();
  UpdateWindow(hwnd);
  SetTimer(hwnd, 0, 100, NULL);
  return hwnd;
}
