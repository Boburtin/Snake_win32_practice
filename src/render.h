#pragma once

#define WIN32_LEAN_AND_MEAN
#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>

#include "snake.h"

struct RenderContext {
    ID2D1Factory *pFactory;
    ID2D1HwndRenderTarget *pRT;
    IDWriteFactory *pDWFactory;
    ID2D1SolidColorBrush *pBrush;
    IDWriteTextFormat *pHudFont;
    IDWriteTextFormat *pHeaderFont;
    void release() {
        pHeaderFont->Release();
        pHudFont->Release();
        pBrush->Release();
        pDWFactory->Release();
        pRT->Release();
        pFactory->Release();
    }
};

void InitRenderContext(RenderContext &ctx, HWND hwnd);

void PaintGame(RenderContext &ctx, Snake &snake, GameData &gd);

void PaintHome(RenderContext &ctx, GameData &gd);

void PaintMenu(RenderContext &ctx, GameData &gd);

HWND WindowInit(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);