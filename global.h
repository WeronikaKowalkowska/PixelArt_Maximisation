//
// Created by mochi on 8.10.2026.
//

#ifndef MMPX_GLOBAL_H
#define MMPX_GLOBAL_H

#include <iosfwd>

#include "func.h"

// Minimalna długość linii w pikselach, aby odrzucić szum (np. min. 3 piksele)
const int MIN_LINE_LENGTH = 3;

struct color {
    int r, g, b;
    float a;

    bool operator==(const color &other) {
        return r == other.r && g == other.g && b == other.b&&a==other.a;
    }

    bool operator<(const color &other) const {
        if (r != other.r) return r < other.r;
        if (g != other.g) return g < other.g;
        return b < other.b;
    }

    bool operator==(const color & other) const = default;
};

struct Pixel {
    int x;
    int y;
    color c;

    bool operator==(const Pixel &other) {
        return x == other.x && y == other.y&&c==other.c;
    }

    bool operator==(long i) const;
};

struct Image {
    int width;
    int height;
    std::vector<Pixel> pixels;

    bool operator==(const Image &other) {
    return width == other.width&&height == other.height&&pixels.size()==other.pixels.size();
    }
};

struct Line {
    color c;

    //int pixels = 0;
    int stairs;

    std::vector<int>thickness_x ;
    std::vector<Pixel> pixels;

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
SDL_Surface* mmpx_surface = NULL;

bool window_done = false;
SDL_Window *window;

Image img_before_scaling;
Image img_resized;
Image mmpx_img;

std::vector<color> original_palette;
std::vector<color> new_palette;

std::vector<float> original_alphas;
std::vector<float> new_alphas;

std::vector<Line> original_lines;
std::vector<Line> scaled_lines;


///debug only

std::vector<Pixel> concave_original;
std::vector<Pixel> convex_original;
std::vector<Pixel> concave_scaled;
std::vector<Pixel> convex_scaled;
std::vector<Pixel> intersections_original;
std::vector<Pixel> intersections_scaled;

///

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

SDL_Surface* image_to_surface(Image &img) {
    SDL_Surface* surface = SDL_CreateSurface(img.width, img.height, SDL_PIXELFORMAT_ABGR8888);
    if (!surface) {
        SDL_Log("Błąd tworzenia powierzchni SDL: %s", SDL_GetError());
        return nullptr;
    }

    Uint32* dst_pixels = (Uint32*)surface->pixels; //wskaznik do bufora pixeli sdl_surface, uint32 bo pixel w strukturze sdl jest tego typu
    int pitch_pixels = surface->pitch / 4; // szerokosc wiersza w bajtach - dzielimy przez 4, bo Uint32 ma 4 bajty

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            int idx = y * img.width + x;
            if (idx >= img.pixels.size()) continue;
            const Pixel& p = img.pixels[idx];
            Uint8 alpha = (p.c.a <= 1.0f && p.c.a > 0.0f) ? (Uint8)(p.c.a * 255.0f) : (Uint8)p.c.a;
            if (p.c.a == 0.0f) alpha = 0;
            //przetwarzanmie struktury color na format ktory mozna zapakowac do sdl_surafce
            Uint32 packed_color = ((Uint32)(unsigned char)p.c.r << 24) |
                              ((Uint32)(unsigned char)p.c.g << 16) |
                              ((Uint32)(unsigned char)p.c.b << 8)  |
                              (Uint32)alpha;
            dst_pixels[y * pitch_pixels + x] = packed_color;
        }
    }

    return surface;
}

#endif //MMPX_GLOBAL_H
