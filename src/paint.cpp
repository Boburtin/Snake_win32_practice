#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>

#include <cwchar>

#include "BoardLogic.h"

void InitRenderContext(RenderContext &ctx, HWND hwnd) {
    auto makeFont = [&](IDWriteTextFormat *&fmt, const wchar_t *fam, DWRITE_FONT_WEIGHT wght, float sz) {
        ctx.pDWFactory->CreateTextFormat(fam, nullptr, wght, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_EXPANDED, sz,
                                         L"en-us", &fmt);
        fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    };
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &ctx.pFactory);
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
    ctx.pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size),
                                         &ctx.pRT);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        reinterpret_cast<IUnknown **>(&ctx.pDWFactory));
    ctx.pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &ctx.pBrush);
    makeFont(ctx.pHeaderFont, L"Lucida Calligraphy", DWRITE_FONT_WEIGHT_BOLD, TILESIZE * 1.5f);
    makeFont(ctx.pHudFont, L"Segoe Print", DWRITE_FONT_WEIGHT_REGULAR, TILESIZE * 1.f);
    makeFont(ctx.pLockedOptionFont, L"Segoe Print", DWRITE_FONT_WEIGHT_LIGHT, TILESIZE * 1.f);
}

void PaintGame(RenderContext &ctx, const Snake &snake, const GameData &gd) {
    auto tileRect = [](const PVec2 &v) -> D2D1_RECT_F {
        float x = v.x * static_cast<float>(TILESIZE), y = v.y * static_cast<float>(TILESIZE);
        return { x + 1.f, y + 1.f, x + TILESIZE - 1.f, y + TILESIZE - 1.f };
    };

    HudRects h {};
    D2D1_RECT_F foodRect = tileRect(gd.food);
    wchar_t scoreBuf[64];
    wchar_t levelBuf[64];
    swprintf_s(scoreBuf, L"Ate: %d (%d)", gd.eaten, gd.score);
    swprintf_s(levelBuf, L"Level: %d", gd.level);

    auto fillSnake = [&] {
        for (uint32_t i {}; i < snake.len; ++i) {
            D2D1_RECT_F snakeRect = tileRect(snake.body[(snake.head + TOTAL_TILES - i) % TOTAL_TILES]);
            ctx.pBrush->SetColor(i == 0 ? levelTheme(gd.level).head : levelTheme(gd.level).body);
            ctx.pRT->FillRectangle(snakeRect, ctx.pBrush);
        }
    };
    auto fillFood = [&] {
        ctx.pBrush->SetColor(levelTheme(gd.level).food);
        ctx.pRT->FillRectangle(foodRect, ctx.pBrush);
    };
    auto fillHud = [&] {
        ctx.pBrush->SetColor(D2D1::ColorF(1.f, 1.f, 1.f, 1.f));
        ctx.pRT->DrawTextW(scoreBuf, static_cast<UINT32>(wcslen(scoreBuf)), ctx.pHudFont, h.level, ctx.pBrush);
        ctx.pRT->DrawTextW(levelBuf, static_cast<UINT32>(wcslen(levelBuf)), ctx.pHudFont, h.score, ctx.pBrush);
        ctx.pRT->DrawRectangle(h.frame, ctx.pBrush);
    };

    switch (ctx.fullRedraw) {
        case FALSE: {
            D2D1_RECT_F prevTailRect = tileRect(snake.body[(snake.head - snake.len + TOTAL_TILES) % TOTAL_TILES]);
            if (ctx.foodEaten > 0) {
                ctx.foodEaten = FALSE;
                fillFood();
                ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
                ctx.pRT->FillRectangle(h.frame, ctx.pBrush);
                fillHud();
            }
            fillSnake();
            ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
            ctx.pRT->FillRectangle(prevTailRect, ctx.pBrush);
            break;
        }
        case TRUE: {
            ctx.pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));
            ctx.pBrush->SetColor(D2D1::ColorF(.2f, .2f, .2f, .2f));
            for (float col { 0.f }; col <= COLS; ++col)
                ctx.pRT->DrawLine({ col * TILESIZE, 0.f }, { col * TILESIZE, HEIGHT }, ctx.pBrush, .5f);
            for (float row { 0.f }; row <= ROWS; ++row)
                ctx.pRT->DrawLine({ 0.f, row * TILESIZE }, { WIDTH, row * TILESIZE }, ctx.pBrush, .5f);
            fillSnake();
            fillFood();
            fillHud();
        } break;
    }
};

