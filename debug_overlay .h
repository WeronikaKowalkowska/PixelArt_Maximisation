#ifndef MMPX_DEBUG_OVERLAY_H
#define MMPX_DEBUG_OVERLAY_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>

#include "global.h"

void debug_lines(Image debugged,std::vector<Line> lines, std::string filename) {
    Image img=debugged;
    for (Line l : lines) {
        for (Pixel p : l.pixels) {
            img.pixels[p.y*img.width + p.x].c = color{255,255,255,1};
        }
    }
    SDL_Surface *to_save = image_to_surface(img);
    SDL_SavePNG(to_save,filename.c_str());
}

void debug_corners(Image debugged,std::vector<Pixel>debug_convex, std::vector<Pixel>debug_concave,  std::string filename) {
    Image img = debugged;
    for (auto p: debug_convex) {
        img.pixels[p.y*img.width + p.x].c = color{255,255,255,1};
    }
    for (auto p: debug_concave) {
        img.pixels[p.y*img.width + p.x].c = color{255,0,255,1};
    }
    SDL_Surface *to_save = image_to_surface(img);
    SDL_SavePNG(to_save,filename.c_str());
}

void debug_intersections(Image debugged, std::vector<Pixel> intersections,  std::string filename) {
    Image img = debugged;
    for (Pixel p : intersections) {
        img.pixels[p.y*img.width + p.x].c = color{255,255,255,1};
    }
    SDL_Surface *to_save = image_to_surface(img);
    SDL_SavePNG(to_save,filename.c_str());
}

#endif //MMPX_DEBUG_OVERLAY_H
