#pragma once

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

inline LevelTheme levelTheme(int level) {
    auto s = [&](float base) { return fmodf(base + .2f * level, 1.0f); };
    return { { s(.75f), s(.75f), s(0.f), 1.f }, { s(.65f), s(.35f), s(.25f), .9f }, { s(.1f), s(1.f), s(.3f), 1.f } };
}

void InitRenderContext(RenderContext &ctx, HWND hwnd);

void PaintGame(RenderContext &ctx, const Snake &snake, const GameData &gd);

void PaintHomeMenu(RenderContext &ctx, const GameData &gd);

void PaintPauseMenu(RenderContext &ctx, const GameData &gd);

void PaintGameOverMenu(RenderContext &ctx, const GameData &gd);

HWND WindowInit(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);