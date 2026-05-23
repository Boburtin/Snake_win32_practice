#ifndef GAME_H
#define GAME_H

inline constexpr int WIDTH = 800;
inline constexpr int HEIGHT = 600;
inline constexpr int TILESIZE = 20;
inline constexpr int COLS = WIDTH / TILESIZE;
inline constexpr int ROWS = HEIGHT / TILESIZE;
inline constexpr int TOTAL_TILES = ROWS * COLS;
inline constexpr int START_X = COLS / 2;
inline constexpr int START_Y = ROWS / 2;
inline constexpr int START_INDEX = START_Y * COLS + START_X;

inline constexpr float HPOINT = HEIGHT * .1f, HPOINT2 = HEIGHT * .2f, HPOINT3 = HEIGHT * .3f, HPOINT4 = HEIGHT * .4f,
                       HPOINT5 = HEIGHT * .5f, HPOINT6 = HEIGHT * .6f, HPOINT7 = HEIGHT * .7f, HPOINT8 = HEIGHT * .8f,
                       HPOINT9 = HEIGHT * .9f;
inline constexpr float WPOINT = WIDTH * .1f, WPOINT2 = WIDTH * .2f, WPOINT3 = WIDTH * .3f, WPOINT4 = WIDTH * .4f,
                       WPOINT5 = WIDTH * .5f, WPOINT6 = WIDTH * .6f, WPOINT7 = WIDTH * .7f, WPOINT8 = WIDTH * .8f,
                       WPOINT9 = WIDTH * .9f;

#endif