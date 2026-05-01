#pragma once
#define WIN32_LEAN_AND_MEAN
#include "snake.h"
#include "windows.h"

struct RenderContext {
  HDC hdc;
  PAINTSTRUCT &ps;
};

void PaintGame(RenderContext &ctx, Snake &snake, GameData &gd);

void PaintMenu(RenderContext &ctx, Moption option);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND WindowInit(HINSTANCE hInstance, int nCmdShow);

void UpdateGame();

void InitGame();