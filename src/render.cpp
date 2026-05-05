#include "render.h"

#include <cwchar>

void InitRenderContext(RenderContext &ctx, HWND hwnd) {
    auto makeFont = [&](IDWriteTextFormat *&fmt, const wchar_t *fam, DWRITE_FONT_WEIGHT wght, float sz) {
        ctx.pDWFactory->CreateTextFormat(fam, nullptr, wght, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_EXPANDED, sz,
                                         L"", &fmt);
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
    makeFont(ctx.pHeaderFont, L"Lucida Console", DWRITE_FONT_WEIGHT_BOLD, TILESIZE * 1.5f);
    makeFont(ctx.pHudFont, L"Lucida Console", DWRITE_FONT_WEIGHT_REGULAR, TILESIZE * 1.f);
    ctx.pDWFactory->CreateTextFormat(L"Lucida Console", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_ITALIC,
                                     DWRITE_FONT_STRETCH_NORMAL, TILESIZE * .75f, L"", &ctx.pLockedOptionFont);
    ctx.pLockedOptionFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    ctx.pLockedOptionFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void PaintGame(RenderContext &ctx, const Snake &snake, const GameData &gd) {
    auto tileRect = [](PVec2 v) -> D2D1_RECT_F {
        float x = v.x * TILESIZE, y = v.y * TILESIZE;
        return { x + 1.f, y + 1.f, x + TILESIZE - 1.f, y + TILESIZE - 1.f };
    };
    auto fillSnake = [&] {
        for (int i { 0 }; i < snake.len; ++i) {
            D2D1_RECT_F snakeRect = tileRect(snake.body[(snake.head + TOTAL_TILES - i) % TOTAL_TILES]);
            ctx.pBrush->SetColor(i == 0 ? levelTheme(gd.level).head : levelTheme(gd.level).body);
            ctx.pRT->FillRectangle(snakeRect, ctx.pBrush);
        }
    };
    D2D1_RECT_F hudOutlineRect = { 0.f, HEIGHT * 1.f, WIDTH * 1.f, HEIGHT + TILESIZE * 2.f };
    D2D1_RECT_F hudScoreRect = { 0.f, HEIGHT * 1.f, WIDTH * .5f, HEIGHT + TILESIZE * 2.f };
    D2D1_RECT_F hudLevelRect = { WIDTH * .5f, HEIGHT * 1.f, WIDTH * 1.f, HEIGHT + TILESIZE * 2.f };
    D2D1_RECT_F foodRect = tileRect(gd.food);
    wchar_t scoreBuf[64];
    wchar_t levelBuf[64];
    swprintf_s(scoreBuf, L"pts: %d", gd.score);
    swprintf_s(levelBuf, L"lvl: %d", gd.level);
    switch (ctx.fullRedraw) {
        case FALSE: {
            D2D1_RECT_F prevTailRect = tileRect(snake.body[(snake.head - snake.len + TOTAL_TILES) % TOTAL_TILES]);
            if (ctx.foodEaten > 0) {
                ctx.foodEaten = FALSE;
                ctx.pBrush->SetColor(levelTheme(gd.level).food);
                ctx.pRT->FillRectangle(foodRect, ctx.pBrush);
                ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
                ctx.pRT->FillRectangle(hudOutlineRect, ctx.pBrush);
                ctx.pBrush->SetColor(D2D1::ColorF(.8f, 1.f, .8f, .75f));
                ctx.pRT->DrawTextW(scoreBuf, wcslen(scoreBuf), ctx.pHudFont, hudScoreRect, ctx.pBrush);
                ctx.pRT->DrawTextW(levelBuf, wcslen(levelBuf), ctx.pHudFont, hudLevelRect, ctx.pBrush);
                ctx.pRT->DrawRectangle(hudOutlineRect, ctx.pBrush);
            }
            fillSnake();
            ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
            ctx.pRT->FillRectangle(prevTailRect, ctx.pBrush);
            break;
        }
        case TRUE: {
            ctx.pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));
            ctx.pBrush->SetColor(D2D1::ColorF(.2f, .2f, .2f, .1f));
            for (float col { 0.f }; col <= COLS; ++col)
                ctx.pRT->DrawLine({ col * TILESIZE, 0.f }, { col * TILESIZE, HEIGHT }, ctx.pBrush, .5f);
            for (float row { 0.f }; row <= ROWS; ++row)
                ctx.pRT->DrawLine({ 0.f, row * TILESIZE }, { WIDTH, row * TILESIZE }, ctx.pBrush, .5f);
            fillSnake();
            ctx.pBrush->SetColor(levelTheme(gd.level).food);
            ctx.pRT->FillRectangle(foodRect, ctx.pBrush);
            ctx.pBrush->SetColor(D2D1::ColorF(.8f, 1.f, .8f, .75f));
            ctx.pRT->DrawTextW(scoreBuf, wcslen(scoreBuf), ctx.pHudFont, hudScoreRect, ctx.pBrush);
            ctx.pRT->DrawTextW(levelBuf, wcslen(levelBuf), ctx.pHudFont, hudLevelRect, ctx.pBrush);
            ctx.pRT->DrawRectangle(hudOutlineRect, ctx.pBrush);
        } break;
    }
};

template <typename T>
void drawOption(RenderContext &ctx, const T &gdOption, const T thisOption, const D2D1_RECT_F &rect,
                const wchar_t *label) {
    ctx.pBrush->SetColor(D2D1::ColorF(thisOption == gdOption ? D2D1::ColorF::White : D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(rect, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(thisOption == gdOption ? D2D1::ColorF::Black : D2D1::ColorF::White));
    ctx.pRT->DrawTextW(label, wcslen(label), ctx.pHudFont, rect, ctx.pBrush);
}

void drawMenuFrame(RenderContext &ctx, const D2D1_RECT_F &mR, const D2D1_RECT_F &mH, const wchar_t *title) {
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(mR, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    ctx.pRT->DrawRectangle(mR, ctx.pBrush);
    ctx.pRT->DrawTextW(title, wcslen(title), ctx.pHeaderFont, mH, ctx.pBrush);
}

void PaintHomeMenu(RenderContext &ctx, const GameData &gd) {
    D2D1_RECT_F mR = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .7f };
    D2D1_RECT_F mH = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .3f };
    D2D1_RECT_F m1 = { WIDTH * .2f, HEIGHT * .3f, WIDTH * .8f, HEIGHT * .4f };
    D2D1_RECT_F m2 = { WIDTH * .2f, HEIGHT * .4f, WIDTH * .8f, HEIGHT * .5f };
    D2D1_RECT_F m3 = { WIDTH * .2f, HEIGHT * .5f, WIDTH * .8f, HEIGHT * .6f };
    D2D1_RECT_F m4 = { WIDTH * .2f, HEIGHT * .6f, WIDTH * .8f, HEIGHT * .7f };
    drawMenuFrame(ctx, mR, mH, L"* Home *");
    drawOption(ctx, gd.homeMenu, HomeMenu::PLAY, m1, L"Play");
    ctx.pBrush->SetColor(gd.homeMenu == HomeMenu::MULTI ? D2D1::ColorF(D2D1::ColorF::LightGray)
                                                        : D2D1::ColorF(D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(m2, ctx.pBrush);
    ctx.pBrush->SetColor(gd.homeMenu == HomeMenu::TOGGLE ? D2D1::ColorF(D2D1::ColorF::LightGray)
                                                         : D2D1::ColorF(D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(m3, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Gray, .5f));
    ctx.pRT->DrawTextW(L"Multi", wcslen(L"Multi"), ctx.pLockedOptionFont, m2, ctx.pBrush);
    ctx.pRT->DrawTextW(L"Toggle", wcslen(L"Toggle"), ctx.pLockedOptionFont, m3, ctx.pBrush);
    drawOption(ctx, gd.homeMenu, HomeMenu::QUIT, m4, L"Quit");
}

void PaintPauseMenu(RenderContext &ctx, const GameData &gd) {
    D2D1_RECT_F mR = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .7f };
    D2D1_RECT_F mH = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .3f };
    D2D1_RECT_F m1 = { WIDTH * .2f, HEIGHT * .3f, WIDTH * .8f, HEIGHT * .4f };
    D2D1_RECT_F m2 = { WIDTH * .2f, HEIGHT * .4f, WIDTH * .8f, HEIGHT * .5f };
    D2D1_RECT_F m3 = { WIDTH * .2f, HEIGHT * .5f, WIDTH * .8f, HEIGHT * .6f };
    D2D1_RECT_F m4 = { WIDTH * .2f, HEIGHT * .6f, WIDTH * .8f, HEIGHT * .7f };
    drawMenuFrame(ctx, mR, mH, L"* Paused *");
    drawOption(ctx, gd.pauseMenu, PauseMenu::RESUME, m1, L"Resume");
    drawOption(ctx, gd.pauseMenu, PauseMenu::RESTART, m2, L"Restart");
    drawOption(ctx, gd.pauseMenu, PauseMenu::BACK, m3, L"Back");
    drawOption(ctx, gd.pauseMenu, PauseMenu::QUIT, m4, L"Quit");
}

void PaintGameOverMenu(RenderContext &ctx, const GameData &gd) {
    D2D1_RECT_F mR = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .6f };
    D2D1_RECT_F mH = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .3f };
    D2D1_RECT_F m1 = { WIDTH * .2f, HEIGHT * .3f, WIDTH * .8f, HEIGHT * .4f };
    D2D1_RECT_F m2 = { WIDTH * .2f, HEIGHT * .4f, WIDTH * .8f, HEIGHT * .5f };
    D2D1_RECT_F m3 = { WIDTH * .2f, HEIGHT * .5f, WIDTH * .8f, HEIGHT * .6f };
    drawMenuFrame(ctx, mR, mH, L"* Game over *");
    drawOption(ctx, gd.gameOverMenu, GameOverMenu::RESTART, m1, L"Restart");
    drawOption(ctx, gd.gameOverMenu, GameOverMenu::BACK, m2, L"Back");
    drawOption(ctx, gd.gameOverMenu, GameOverMenu::QUIT, m3, L"Quit");
}