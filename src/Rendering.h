#ifndef RENDER_H
#define RENDER_H

#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>
#include <cstdint>
#include <cmath>
#include <cwchar>

inline constexpr int WIDTH = 800;
inline constexpr int HEIGHT = 600;

inline constexpr float HPOINT = HEIGHT * .1f, HPOINT2 = HEIGHT * .2f, HPOINT3 = HEIGHT * .3f, HPOINT4 = HEIGHT * .4f,
                       HPOINT5 = HEIGHT * .5f, HPOINT6 = HEIGHT * .6f, HPOINT7 = HEIGHT * .7f, HPOINT8 = HEIGHT * .8f,
                       HPOINT9 = HEIGHT * .9f;
inline constexpr float WPOINT = WIDTH * .1f, WPOINT2 = WIDTH * .2f, WPOINT3 = WIDTH * .3f, WPOINT4 = WIDTH * .4f,
                       WPOINT5 = WIDTH * .5f, WPOINT6 = WIDTH * .6f, WPOINT7 = WIDTH * .7f, WPOINT8 = WIDTH * .8f,
                       WPOINT9 = WIDTH * .9f;

inline constexpr int TILESIZE = 20;

struct RenderContext {
    ID2D1Factory *pFactory;
    ID2D1HwndRenderTarget *pRT;
    IDWriteFactory *pDWFactory;
    ID2D1SolidColorBrush *pBrush;
    IDWriteTextFormat *pHudFont;
    IDWriteTextFormat *pHeaderFont;
    IDWriteTextFormat *pLockedOptionFont;
    BOOL fullRedraw = TRUE;
    BOOL foodEaten = FALSE;
    void release() {
        pLockedOptionFont->Release();
        pHeaderFont->Release();
        pHudFont->Release();
        pBrush->Release();
        pDWFactory->Release();
        pRT->Release();
        pFactory->Release();
    }
};

struct MenuOption {
    const D2D1_RECT_F rect;
    const wchar_t *label;
    const bool locked = false;
};

struct MenuRect {
    const D2D1_RECT_F frame;
    const D2D1_RECT_F header;
    const wchar_t *title;
    const MenuOption options[4];
    const uint32_t count;
};

inline const MenuRect gameOverMenu { { WPOINT2, HPOINT2, WPOINT8, HPOINT5 },
                                     { WPOINT2, HPOINT2, WPOINT8, HPOINT3 },
                                     L"* Game Over *",
                                     { { { WPOINT2, HPOINT3, WPOINT8, HPOINT4 }, L"redo", false },
                                       { { WPOINT2, HPOINT4, WPOINT8, HPOINT5 }, L"quit", false } },
                                     2 };

inline const MenuRect homeMenu { { WPOINT2, HPOINT2, WPOINT8, HPOINT7 },
                                 { WPOINT2, HPOINT2, WPOINT8, HPOINT3 },
                                 L"* Home Menu *",
                                 { { { WPOINT2, HPOINT3, WPOINT8, HPOINT4 }, L"start", false },
                                   { { WPOINT2, HPOINT4, WPOINT8, HPOINT5 }, L"multi", true },
                                   { { WPOINT2, HPOINT5, WPOINT8, HPOINT6 }, L"toggle ", true },
                                   { { WPOINT2, HPOINT6, WPOINT8, HPOINT7 }, L"leave", false } },
                                 4 };

inline const MenuRect pauseMenu { { WPOINT2, HPOINT2, WPOINT8, HPOINT6 },
                                  { WPOINT2, HPOINT2, WPOINT8, HPOINT3 },
                                  L"* Paused *",
                                  { { { WPOINT2, HPOINT3, WPOINT8, HPOINT4 }, L"play", false },
                                    { { WPOINT2, HPOINT4, WPOINT8, HPOINT5 }, L"redo", false },
                                    { { WPOINT2, HPOINT5, WPOINT8, HPOINT6 }, L"quit   ", false } },
                                  3 };

struct LevelTheme {
    D2D1_COLOR_F head;
    D2D1_COLOR_F body;
    D2D1_COLOR_F food;
};

struct HudRects {
    D2D1_RECT_F frame = { 0.f, HEIGHT * 1.f, WIDTH * 1.f, HEIGHT + TILESIZE * 2.f };
    D2D1_RECT_F score = { 0.f, HEIGHT * 1.f, WIDTH * .5f, HEIGHT + TILESIZE * 2.f };
    D2D1_RECT_F level = { WIDTH * .5f, HEIGHT * 1.f, WIDTH * 1.f, HEIGHT + TILESIZE * 2.f };
};

inline LevelTheme levelTheme(int level) {
    auto s = [level](float base) { return fmodf(base + .1f * level, 1.f); };
    return { { s(.75f), .75f, 0.f, 1.f }, { 1.f, 1.f, 1.f, .75f }, { s(.1f), 1.f, s(0.f), 1.f } };
}

template <typename T>
inline void drawMenuSection(RenderContext &ctx, T currentSelection, uint8_t thisIndex, const MenuRect &mr) {
    ctx.pBrush->SetColor(D2D1::ColorF(static_cast<uint8_t>(currentSelection) == thisIndex ? D2D1::ColorF::White : D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(mr.options[thisIndex].rect, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(mr.options[thisIndex].locked    ? D2D1::ColorF::Gray
                                      : static_cast<uint8_t>(currentSelection) == thisIndex ? D2D1::ColorF::Black
                                                                      : D2D1::ColorF::White));
    ctx.pRT->DrawTextW(mr.options[thisIndex].label, static_cast<uint32_t>(wcslen(mr.options[thisIndex].label)),
                       mr.options[thisIndex].locked ? ctx.pLockedOptionFont : ctx.pHudFont, mr.options[thisIndex].rect,
                       ctx.pBrush);
}

template <typename T>
inline void PaintMenu(RenderContext &ctx, const MenuRect &mr, T currentSelection) {
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(mr.frame, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    ctx.pRT->DrawRectangle(mr.frame, ctx.pBrush);
    ctx.pRT->DrawTextW(mr.title, static_cast<uint32_t>(wcslen(mr.title)), ctx.pHeaderFont, mr.header, ctx.pBrush);
    for (uint8_t i {}; i < mr.count; ++i) { drawMenuSection(ctx, currentSelection, i, mr); }
}

#endif
