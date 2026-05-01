#pragma once

#define WIN32_LEAN_AND_MEAN
#include "snake.h"
#include "windows.h"
#include <d2d1.h>
#include <dwrite.h>

struct RenderContext {
  ID2D1Factory *pFactory;
  ID2D1HwndRenderTarget *pRT;
  IDWriteFactory *pDWFactory;
  ID2D1SolidColorBrush *pBrush;
  IDWriteTextFormat *pHudFont;
  IDWriteTextFormat *pHeaderFont;
};

void InitRenderContext(RenderContext &ctx, HWND hwnd);

void PaintGame(RenderContext &ctx, Snake &snake, GameData &gd);

void PaintMenu(RenderContext &ctx, Moption option);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND WindowInit(HINSTANCE hInstance, int nCmdShow);