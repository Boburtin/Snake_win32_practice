#include "render.h"

#include <string>

void InitRenderContext(RenderContext &ctx, HWND hwnd) {
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &ctx.pFactory);
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
    ctx.pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size),
                                         &ctx.pRT);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        reinterpret_cast<IUnknown **>(&ctx.pDWFactory));
    ctx.pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &ctx.pBrush);
    ctx.pDWFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_ITALIC,
                                     DWRITE_FONT_STRETCH_NORMAL, TILESIZE * 2.f, L"", &ctx.pHeaderFont);
    ctx.pHeaderFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    ctx.pHeaderFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    ctx.pDWFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_ITALIC,
                                     DWRITE_FONT_STRETCH_NORMAL, TILESIZE * 1.5f, L"", &ctx.pHudFont);
    ctx.pHudFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    ctx.pHudFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void PaintGame(RenderContext &ctx, Snake &snake, GameData &gd) {
    auto tileRect = [](PVec2 v) -> D2D1_RECT_F {
        return {(float)v.x * TILESIZE, (float)v.y * TILESIZE, (float)v.x * TILESIZE + TILESIZE,
                (float)v.y * TILESIZE + TILESIZE};
    };
    ctx.pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    for (int i {0}; i < snake.len; ++i) {
        D2D1_RECT_F snakeRect = tileRect(snake.body[(snake.head + TOTAL_TILES - i) % TOTAL_TILES]);
        ctx.pBrush->SetColor(i == 0 ? D2D1::ColorF(D2D1::ColorF::White) : D2D1::ColorF(0.75f, 0.75f, 0.75f));
        ctx.pRT->FillRectangle(snakeRect, ctx.pBrush);
    }
    D2D1_RECT_F foodRect = tileRect(gd.food);
    ctx.pBrush->SetColor(D2D1::ColorF(0, 1, 0));
    ctx.pRT->FillRectangle(foodRect, ctx.pBrush);
    D2D1_RECT_F hudRect = {0.f, (float)HEIGHT, (float)WIDTH, (HEIGHT + TILESIZE * 2.f)};
    wchar_t buf[16];
    swprintf_s(buf, L"Score: %d", snake.len);
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    ctx.pRT->DrawRectangle(hudRect, ctx.pBrush);
    ctx.pRT->DrawTextW(buf, wcslen(buf), ctx.pHudFont, hudRect, ctx.pBrush);
};

template <typename T>
void drawOption(RenderContext &ctx, T &gdOption, T thisOption, D2D1_RECT_F &rect, const wchar_t *label) {
    ctx.pBrush->SetColor(D2D1::ColorF(thisOption == gdOption ? D2D1::ColorF::White : D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(rect, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(thisOption == gdOption ? D2D1::ColorF::Black : D2D1::ColorF::White));
    ctx.pRT->DrawText(label, wcslen(label), ctx.pHudFont, rect, ctx.pBrush);
}

void PaintHome(RenderContext &ctx, GameData &gd) {
    D2D1_RECT_F mR = {WIDTH * 0.2f, HEIGHT * 0.2f, WIDTH * 0.8f, HEIGHT * 0.8f};
    D2D1_RECT_F mH = {WIDTH * 0.2f, HEIGHT * 0.2f, WIDTH * 0.8f, HEIGHT * 0.3f};
    D2D1_RECT_F m1 = {WIDTH * 0.2f, HEIGHT * 0.3f, WIDTH * 0.8f, HEIGHT * 0.4f};
    D2D1_RECT_F m2 = {WIDTH * 0.2f, HEIGHT * 0.4f, WIDTH * 0.8f, HEIGHT * 0.5f};
    D2D1_RECT_F m3 = {WIDTH * 0.2f, HEIGHT * 0.5f, WIDTH * 0.8f, HEIGHT * 0.6f};
    D2D1_RECT_F m4 = {WIDTH * 0.2f, HEIGHT * 0.6f, WIDTH * 0.8f, HEIGHT * 0.7f};
    D2D1_RECT_F m5 = {WIDTH * 0.2f, HEIGHT * 0.7f, WIDTH * 0.8f, HEIGHT * 0.8f};
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(mR, ctx.pBrush);
    ctx.pRT->FillRectangle(mH, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    ctx.pRT->DrawRectangle(mR, ctx.pBrush);
    ctx.pRT->DrawTextW(L"Main Menu", wcslen(L"Main Menu"), ctx.pHeaderFont, mH, ctx.pBrush);
    drawOption(ctx, gd.mHome, M_Home::SOLO, m1, L"Solo");
    drawOption(ctx, gd.mHome, M_Home::MULTI, m2, L"Multi");
    drawOption(ctx, gd.mHome, M_Home::SCORES, m3, L"Scores");
    drawOption(ctx, gd.mHome, M_Home::SETTINGS, m4, L"Settings");
    drawOption(ctx, gd.mHome, M_Home::EXIT, m5, L"Exit");
}

void PaintMenu(RenderContext &ctx, GameData &gd) {
    D2D1_RECT_F mR = {WIDTH * 0.25f, HEIGHT * 0.25f, WIDTH * 0.75f, HEIGHT * 0.75f};
    D2D1_RECT_F mH = {WIDTH * 0.25f, HEIGHT * 0.25f, WIDTH * 0.75f, HEIGHT * 0.375f};
    D2D1_RECT_F m1 = {WIDTH * 0.25f, HEIGHT * 0.375f, WIDTH * 0.75f, HEIGHT * 0.5f};
    D2D1_RECT_F m2 = {WIDTH * 0.25f, HEIGHT * 0.5f, WIDTH * 0.75f, HEIGHT * 0.625f};
    D2D1_RECT_F m3 = {WIDTH * 0.25f, HEIGHT * 0.625f, WIDTH * 0.75f, HEIGHT * 0.75f};
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(mR, ctx.pBrush);
    ctx.pRT->FillRectangle(mH, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    ctx.pRT->DrawRectangle(mR, ctx.pBrush);
    ctx.pRT->DrawTextW(L"Paused", wcslen(L"Paused"), ctx.pHeaderFont, mH, ctx.pBrush);
    drawOption(ctx, gd.mOption, M_Paused::RESUME, m1, L"Resume");
    drawOption(ctx, gd.mOption, M_Paused::RESTART, m2, L"Restart");
    drawOption(ctx, gd.mOption, M_Paused::QUIT, m3, L"Quit");
}
