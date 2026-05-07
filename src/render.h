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
    void release() {
        pLockedOptionFont->Release();
        pHeaderFont->Release();
        pHudFont->Release();
        pBrush->Release();
        pDWFactory->Release();
        pRT->Release();
        pFactory->Release();
    }
    BOOL fullRedraw = TRUE;
    BOOL foodEaten = FALSE;
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

struct HomeMenuRects {
    D2D1_RECT_F frame = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .7f };
    D2D1_RECT_F header = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .3f };
    D2D1_RECT_F play = { WIDTH * .2f, HEIGHT * .3f, WIDTH * .8f, HEIGHT * .4f };
    D2D1_RECT_F multi = { WIDTH * .2f, HEIGHT * .4f, WIDTH * .8f, HEIGHT * .5f };
    D2D1_RECT_F toggle = { WIDTH * .2f, HEIGHT * .5f, WIDTH * .8f, HEIGHT * .6f };
    D2D1_RECT_F quit = { WIDTH * .2f, HEIGHT * .6f, WIDTH * .8f, HEIGHT * .7f };
};

struct PauseMenuRects {
    D2D1_RECT_F frame = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .7f };
    D2D1_RECT_F header = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .3f };
    D2D1_RECT_F resume = { WIDTH * .2f, HEIGHT * .3f, WIDTH * .8f, HEIGHT * .4f };
    D2D1_RECT_F restart = { WIDTH * .2f, HEIGHT * .4f, WIDTH * .8f, HEIGHT * .5f };
    D2D1_RECT_F back = { WIDTH * .2f, HEIGHT * .5f, WIDTH * .8f, HEIGHT * .6f };
    D2D1_RECT_F quit = { WIDTH * .2f, HEIGHT * .6f, WIDTH * .8f, HEIGHT * .7f };
};

struct GameOverMenuRects {
    D2D1_RECT_F frame = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .6f };
    D2D1_RECT_F header = { WIDTH * .2f, HEIGHT * .2f, WIDTH * .8f, HEIGHT * .3f };
    D2D1_RECT_F restart = { WIDTH * .2f, HEIGHT * .3f, WIDTH * .8f, HEIGHT * .4f };
    D2D1_RECT_F back = { WIDTH * .2f, HEIGHT * .4f, WIDTH * .8f, HEIGHT * .5f };
    D2D1_RECT_F quit = { WIDTH * .2f, HEIGHT * .5f, WIDTH * .8f, HEIGHT * .6f };
};

inline LevelTheme levelTheme(int level) {
    auto s = [&](float base) { return fmodf(base + .1f * level, 1.f); };
    return { { s(.75f), .75f, 0.f, 1.f }, { 1.f, 1.f, 1.f, .75f }, { s(.1f), 1.f, .1f, 1.f } };
}

void InitRenderContext(RenderContext &ctx, HWND hwnd);

void PaintGame(RenderContext &ctx, const Snake &snake, const GameData &gd);

void PaintHomeMenu(RenderContext &ctx, const GameData &gd);

void PaintPauseMenu(RenderContext &ctx, const GameData &gd);

void PaintGameOverMenu(RenderContext &ctx, const GameData &gd);

HWND WindowInit(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif