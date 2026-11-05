#ifndef FUNC_H
#define FUNC_H

#include <numeric>
#include <vector>
#include <cmath>

#include "global.h"

//min-max
static float normalize(float n, float n_min, float n_max) {
    return (n - n_min) / (n_max - n_min);
}

inline void get_matrix(Pixel matrix[3][3], Image img, int x, int y) {
    matrix[1][1] = img.pixels[y * img.width + x];
    matrix[0][0] = img.pixels[(y - 1) * img.width + x - 1];
    matrix[0][1] = img.pixels[(y) * img.width + x - 1];
    matrix[0][2] = img.pixels[(y + 1) * img.width + x - 1];
    matrix[1][0] = img.pixels[(y - 1) * img.width + x];
    matrix[1][2] = img.pixels[(y + 1) * img.width + x];
    matrix[2][0] = img.pixels[(y - 1) * img.width + x + 1];
    matrix[2][1] = img.pixels[(y) * img.width + x + 1];
    matrix[2][2] = img.pixels[(y + 1) * img.width + x + 1];
}

bool check_same_color(color c1, color c2) {
    if (c1.r == c2.r &&
    c1.g == c2.g &&
    c1.b == c2.b) {
        return true;
    }
    return false;
}

std::vector<color> getImagePalette(Image &img) {
    std::vector<color> palette;
    for (int i=0; i<img.pixels.size(); i++) {
        color img_c = img.pixels[i].c;
        bool is_in_palette = false;
        for (int j=0; j<palette.size(); j++) {
            if (check_same_color(img_c, palette[j])) {
                is_in_palette = true;
                break;
            }
        }
        if (!is_in_palette) {
            palette.push_back(img_c);
        }
    }
    return palette;
}

static bool is_start(Image img, Pixel p, float dir_x, float dir_y) {
    if (p.x + dir_x < 0 || p.x + dir_x >= img.width || p.y + dir_y < 0 || p.y + dir_y >= img.height) return false;
    return p.c == img.pixels[(int(p.y + dir_y)) * img.width + int(p.x + dir_x)].c;
}

SDL_Color getPixelColor(SDL_Surface* surface, int x, int y) {
    Uint8 r, g, b,a;
    bool success = SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a);
    if (success) {
        SDL_Color c;
        c.r = r;
        c.g = g;
        c.b = b;
        c.a = a;

        return c;
    }
    else {
        //logFile<< "Could not get pixel color"<<std::endl;
        return SDL_Color();
    }
}

bool validate_color (SDL_Color c) {
    if (c.r < 0 || c.r > 255 || c.g < 0 || c.g > 255 ||c.b < 0 || c.b > 255) {
        return true;
    }
    return false;
}

//zapobieganie zbieraniu pixeli z tla
inline bool is_edge_pixel(const Image &img, int x, int y) {
    color c = img.pixels[y * img.width + x].c;
    int neighbors[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    for (int i = 0; i < 4; i++) {
        int nx = x + neighbors[i][0];
        int ny = y + neighbors[i][1];
        if (nx >= 0 && nx < img.width && ny >= 0 && ny < img.height) {
            if (!(img.pixels[ny * img.width + nx].c == c)) {
                return true;
            }
        }
    }
    return false;
}


//all calls are unreachable
static Line count_line(Image &img, Pixel p1, int dir_x, int dir_y, std::vector<std::vector<bool> > &visited) {
    //int len = 2;
    Line l;
    l.startX = p1.x;
    l.startY = p1.y;
    l.c = p1.c;


    Pixel curr = p1;
    while (true) {
        l.pixels.push_back(curr);
        visited[curr.x][curr.y] = true;

        int next_x = curr.x + dir_x;
        int next_y = curr.y + dir_y;

        if (next_x >= 0 && next_x < img.width && next_y >= 0 && next_y < img.height) {
            Pixel next_p = img.pixels[next_y * img.width + next_x];
            next_p.x = next_x;
            next_p.y = next_y;

            if (!visited[next_x][next_y] && (next_p.c == curr.c) && is_edge_pixel(img, next_x, next_y)) {
                curr = next_p;
                continue;
            }
        }

        l.endX = curr.x;
        l.endY = curr.y;
        break;
    }
    int dx = abs(l.endX - l.startX);
    int dy = abs(l.endY - l.startY);
    l.stairs = std::gcd(dx, dy);
                return l;
            }

//oblicz ilosc schodkow w zaleznosci od pozycji poczatkowej i koncowej
static void count_diagonal_lines(Image img, std::vector<Line> &lines)
{

    lines.clear();

    std::vector<std::vector<bool>> visitedDownRight(img.width, std::vector<bool>(img.height, false));
    std::vector<std::vector<bool>> visitedDownLeft(img.width, std::vector<bool>(img.height, false));
    std::vector<std::vector<bool>> visitedHorizontal(img.width, std::vector<bool>(img.height, false));
    std::vector<std::vector<bool>> visitedVertical(img.width, std::vector<bool>(img.height, false));


    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Pixel p = img.pixels[y * img.width + x];

            p.x = x;
            p.y = y;

            // ignorowanie tla
            if (!is_edge_pixel(img, x, y)) continue;

            //dol prawo 1,1
            if (!visitedDownRight[x][y]) {
                Line l = count_line(img, p, 1, 1, visitedDownRight);
                if (l.pixels.size() >= MIN_LINE_LENGTH) {
                    lines.push_back(l);
                }
            }

            // dol lewo -1,1
            if (!visitedDownLeft[x][y]) {
                Line l = count_line(img, p, -1, 1, visitedDownLeft);
                if (l.pixels.size() >= MIN_LINE_LENGTH) {
                    lines.push_back(l);
                }
            }

            // prawo 1,0
            if (!visitedHorizontal[x][y]) {
                Line l = count_line(img, p, 1, 0, visitedHorizontal);
                if (l.pixels.size() >= MIN_LINE_LENGTH) {
                    lines.push_back(l);
                }
            }

            // dol, 0,1
            if (!visitedVertical[x][y]) {
                Line l = count_line(img, p, 0, 1, visitedVertical);
                if (l.pixels.size() >= MIN_LINE_LENGTH) {
                    lines.push_back(l);
                }
            }


        }
    }
}

#endif
