#pragma once

#include "Snakedefs.h"

struct RenderContext;

void PaintGame(RenderContext &ctx, const Snake &snake, const GameData &gd);

void PaintMenu(RenderContext &ctx, Moption option);
