//
// Created by mochi on 8.10.2026.
//

#ifndef MMPX_GLOBAL_H
#define MMPX_GLOBAL_H

#include "func.h"

struct color {
    int r, g, b;
    float a;

    bool operator==(const color &other) {
        return r == other.r && g == other.g && b == other.b;
    }
};

struct Pixel {
    int x;
    int y;
    color c;

    bool operator==(const Pixel &other) {
        return x == other.x && y == other.y&&c==other.c;
    }
};

struct Image {
    int width;
    int height;
    std::vector<Pixel> pixels;
};

struct Line {
    color c;

    int pixels = 0;
    int stairs;

    std::vector<int>thickness_x ;

    int startX;
    int startY;

    int endX;
    int endY;
};

struct MetricsResult {
    float palette;
    float alpha;
    float diagonals;
    float intersections;
    float thickness;
    float corners;
    float average;
};

struct Dot {
    int x;
    int y;
    color c;
    int size_x;
    int size_y;
};

extern std::ofstream logFile;
extern std::ofstream outputFile;

SDL_Surface* surface_before_scaling = NULL;
SDL_Surface* surface_resized = NULL;

bool window_done = false;
SDL_Window *window;

Image img_before_scaling;
Image img_resized;

std::vector<color> original_palette;
std::vector<color> new_palette;

std::vector<float> original_alphas;
std::vector<float> new_alphas;

std::vector<Line> original_lines;
std::vector<Line> scaled_lines;

float p_rate = 1;
int p_difference = 0;

//od gornego lewego rogu w dol
static float directions_down[3][2] =
{
    {1, 0},
    {0, 1},
    {1, 1}
};

//od dolnego lewego rogu w gore
static float directions_up[3][2] =
{
    {0, 1},
    {-1, 0},
    {-1, 1}
};

const int convexMasks[4][3][3] =
{
    {
        {-1,-1,1},
        {-1,1,0},
        {1,0,-1}
    },
    {
                {1,-1,-1},
                {0,1,-1},
                {-1,0,1}
    },
    {
                {-1,0,1},
                {0,1,-1},
                {1,-1,-1}
    },
    {
                {1,0,-1},
                {-1,1,0},
                {-1,-1,1}
    }
};

const int concaveMasks[4][3][3] =
{
    {
        {-1,-1,0},
        {-1,0,1},
        {0,1,-1}
    },
    {
                {0,-1,-1},
                {1,0,-1},
                {-1,1,0}
    },
    {
                {-1,1,0},
                {1,0,-1},
                {0,-1,-1}
    },
    {
                {0,1,-1},
                {-1,0,1},
                {-1,-1,0}
    }
};

MetricsResult res;

#endif //MMPX_GLOBAL_H
