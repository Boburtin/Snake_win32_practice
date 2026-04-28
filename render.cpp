#include "render.h"

#include "windows.h"

struct RenderContext {
  HDC hdc;
  const PAINTSTRUCT &ps;
};

void PaintGame(RenderContext &ctx, const Snake &snake, GameData &gd) {
  auto tileRect = [](PVec2 v) -> RECT { return {v.x * TILESIZE, v.y * TILESIZE, v.x * TILESIZE + TILESIZE, v.y * TILESIZE + TILESIZE}; };
  FillRect(ctx.hdc, &ctx.ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
  FrameRect(ctx.hdc, &ctx.ps.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
  for (int i{0}; i < snake.len; ++i) {
    RECT snakeRect = tileRect(snake.body[(snake.head + TOTAL_TILES - i) % TOTAL_TILES]);
    FillRect(ctx.hdc, &snakeRect, (HBRUSH)GetStockObject(i == 0 ? WHITE_BRUSH : LTGRAY_BRUSH));
  }
  RECT foodRect = tileRect(gd.food);
  HBRUSH foodBrush = CreateSolidBrush(snake.len % 32 < 10 ? RGB(0, 255, 0) : RGB(150, 50, 150));
  FillRect(ctx.hdc, &foodRect, foodBrush);
  DeleteObject(foodBrush);
};

void PaintMenu(RenderContext &ctx, Moption option) {
  auto drawOption = [&](Moption thisOption, RECT &rect, const wchar_t *label) {
    SetTextColor(ctx.hdc, (option == thisOption ? RGB(0, 150, 255) : RGB(255, 255, 255)));
    DrawText(ctx.hdc, label, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  };
  RECT mParent = {WIDTH / 4, HEIGHT / 4, WIDTH * 3 / 4, HEIGHT * 3 / 4};
  RECT mHeader = {WIDTH / 4, HEIGHT / 4, WIDTH * 3 / 4, HEIGHT * 3 / 8};
  RECT mOne = {WIDTH / 4, HEIGHT * 3 / 8, WIDTH * 3 / 4, HEIGHT / 2};
  RECT mTwo = {WIDTH / 4, HEIGHT / 2, WIDTH * 3 / 4, HEIGHT * 5 / 8};
  RECT mThree = {WIDTH / 4, HEIGHT * 5 / 8, WIDTH * 3 / 4, HEIGHT * 3 / 4};
  HFONT headerFont = CreateFont(-TILESIZE * 2, 0, 0, 0, 500, TRUE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
  HFONT optionFont = CreateFont(-TILESIZE, 0, 0, 0, 400, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
  FillRect(ctx.hdc, &mParent, (HBRUSH)GetStockObject(BLACK_BRUSH));
  FrameRect(ctx.hdc, &mParent, (HBRUSH)GetStockObject(WHITE_BRUSH));
  HFONT oldFont = (HFONT)SelectObject(ctx.hdc, headerFont);
  SetBkMode(ctx.hdc, TRANSPARENT);
  SetTextColor(ctx.hdc, RGB(255, 255, 255));
  DrawText(ctx.hdc, L"SNAKE MENU", -1, &mParent, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  drawOption(Moption::CONTINUE, mOne, L"CONTINUE");
  drawOption(Moption::RESTART, mTwo, L"RESTART");
  drawOption(Moption::QUIT, mThree, L"QUIT");
  SelectObject(ctx.hdc, oldFont);
  DeleteObject(headerFont);
  DeleteObject(optionFont);
}
