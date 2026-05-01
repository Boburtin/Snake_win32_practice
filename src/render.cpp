#include "render.h"

#include <string>

void InitRenderContext(RenderContext &ctx, HWND hwnd) {
  D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &ctx.pFactory);

  RECT rc;
  GetClientRect(hwnd, &rc);
  D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
  ctx.pFactory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(),
      D2D1::HwndRenderTargetProperties(hwnd, size), &ctx.pRT);

  DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                      reinterpret_cast<IUnknown **>(&ctx.pDWFactory));

  ctx.pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White),
                                 &ctx.pBrush);

  ctx.pDWFactory->CreateTextFormat(
      L"Consolas", nullptr, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_ITALIC,
      DWRITE_FONT_STRETCH_NORMAL, (float(TILESIZE)) * 2.0f, L"", &ctx.pHeaderFont);
  ctx.pHeaderFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
  ctx.pHeaderFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

  ctx.pDWFactory->CreateTextFormat(
      L"Cascadia Mono", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_ITALIC,
      DWRITE_FONT_STRETCH_NORMAL, (float(TILESIZE)) * 1.5f, L"", &ctx.pHudFont);
  ctx.pHudFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
  ctx.pHudFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void PaintGame(RenderContext &ctx, Snake &snake, GameData &gd) {
  auto tileRect = [](PVec2 v) -> D2D1_RECT_F {
    return {(float(v.x * TILESIZE)), (float(v.y * TILESIZE)),
            (float(v.x * TILESIZE + TILESIZE)),
            (float(v.y * TILESIZE + TILESIZE))};
  };
  ctx.pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));
  for (int i{0}; i < snake.len; ++i) {
    D2D1_RECT_F snakeRect =
        tileRect(snake.body[(snake.head + TOTAL_TILES - i) % TOTAL_TILES]);
    ctx.pBrush->SetColor(i == 0 ? D2D1::ColorF(D2D1::ColorF::White)
                                : D2D1::ColorF(0.75f, 0.75f, 0.75f));
    ctx.pRT->FillRectangle(snakeRect, ctx.pBrush);
  }
  D2D1_RECT_F foodRect = tileRect(gd.food);
  ctx.pBrush->SetColor(D2D1::ColorF(0, 1, 0));
  ctx.pRT->FillRectangle(foodRect, ctx.pBrush);
  D2D1_RECT_F hudRect = {0.f, (float)HEIGHT, (float)WIDTH,
                         (float(HEIGHT + 2.f * TILESIZE))};
  wchar_t buf[16];
  swprintf_s(buf, L"Score: %d", snake.len);
  ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
  ctx.pRT->DrawRectangle(hudRect, ctx.pBrush);
  ctx.pRT->DrawTextW(buf, wcslen(buf), ctx.pHudFont, hudRect, ctx.pBrush);
};

void PaintMenu(RenderContext &ctx, Moption option) {
  auto drawOption = [&](Moption thisOption, D2D1_RECT_F &rect,
                        const wchar_t *label) {
    ctx.pBrush->SetColor(D2D1::ColorF(
        thisOption == option ? D2D1::ColorF::White : D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(rect, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(
        thisOption == option ? D2D1::ColorF::Black : D2D1::ColorF::White));
    ctx.pRT->DrawTextW(label, wcslen(label), ctx.pHudFont, rect, ctx.pBrush);
  };
  D2D1_RECT_F mR = {(float(WIDTH / 4)), (float(HEIGHT / 4)),
                    (float(WIDTH * 3 / 4)), (float(HEIGHT * 3 / 4))};
  D2D1_RECT_F mH = {(float(WIDTH / 4)), (float(HEIGHT / 4)),
                    (float(WIDTH * 3 / 4)), (float(HEIGHT * 3 / 8))};
  D2D1_RECT_F m1 = {(float(WIDTH / 4)), (float(HEIGHT * 3 / 8)),
                    (float(WIDTH * 3 / 4)), (float(HEIGHT / 2))};
  D2D1_RECT_F m2 = {(float(WIDTH / 4)), (float(HEIGHT / 2)),
                    (float(WIDTH * 3 / 4)), (float(HEIGHT * 5 / 8))};
  D2D1_RECT_F m3 = {(float(WIDTH / 4)), (float(HEIGHT * 5 / 8)),
                    (float(WIDTH * 3 / 4)), (float(HEIGHT * 3 / 4))};
  ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
  ctx.pRT->FillRectangle(mR, ctx.pBrush);
  ctx.pRT->FillRectangle(mH, ctx.pBrush);
  ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
  ctx.pRT->DrawRectangle(mR, ctx.pBrush);
  ctx.pRT->DrawTextW(L"PAUSED", wcslen(L"PAUSED"), ctx.pHeaderFont, mH,
                     ctx.pBrush);
  drawOption(Moption::CONTINUE, m1, L"CONTINUE");
  drawOption(Moption::RESTART, m2, L"RESTART");
  drawOption(Moption::QUIT, m3, L"EXIT GAME");
}
