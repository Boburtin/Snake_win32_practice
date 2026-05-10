#ifndef RENDER_H
#define RENDER_H

#define WIN32_LEAN_AND_MEAN
#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>

#include <cmath>

#include "snake.h"

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
    const int count;
};

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

inline LevelTheme levelTheme(int level) {
    auto s = [level](float base) { return fmodf(base + .1f * level, 1.f); };
    return { { s(.75f), .75f, 0.f, 1.f }, { 1.f, 1.f, 1.f, .75f }, { s(.1f), 1.f, s(0.f), 1.f } };
}

void InitRenderContext(RenderContext &ctx, HWND hwnd);

void PaintGame(RenderContext &ctx, const Snake &snake, const GameData &gd);

void drawMenuSection(RenderContext &ctx, int currentSelection, int thisIndex, const MenuRect &mr);

template <typename T>
inline void PaintMenu(RenderContext &ctx, const MenuRect &mr, T currentSelection) {
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    ctx.pRT->FillRectangle(mr.frame, ctx.pBrush);
    ctx.pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    ctx.pRT->DrawRectangle(mr.frame, ctx.pBrush);
    ctx.pRT->DrawTextW(mr.title, wcslen(mr.title), ctx.pHeaderFont, mr.header, ctx.pBrush);
    for (int i {}; i < mr.count; ++i) { drawMenuSection(ctx, (int)currentSelection, i, mr); }
}

HWND WindowInit(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif