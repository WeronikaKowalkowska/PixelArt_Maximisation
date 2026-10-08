#include <math.h>

#include "func.h"
#include "global.h"
#include <vector>

//wyszukaj kropki jako pojedyńcze pixele


void find_dots_original(Image img,std::vector<Dot> dots) {
    for (int x=0;x<img.width;x++) {
        for (int y=0;y<img.height;y++) {
            Pixel p = img.pixels[y*img.width+x];
            Pixel matrix[3][3];
            get_matrix(matrix, img, x, y);
            bool is_dot = true;
            for (int i = 0;i<3;i++) {
                for (int j = 0;j<3;j++) {
                    if (i==1&&j==1) continue;
                    else if (matrix[i][j].c==p.c)is_dot = false;
                }

            }
            if (is_dot) {
                Dot dot;
                dot.x = x;
                dot.y = y;
                dot.c = p.c;
                dot.size_x = 1;
                dot.size_y = 1;
                dots.push_back(dot);
            }
        }
    }
}

void find_dots_scaled(Image img,std::vector<Dot> dots, int scale) {
for (int x=0;x<img.width;x++) {
    for (int y=0;y<img.height;y++) {
        Pixel p = img.pixels[y*img.width+x];
        bool is_dot = true;
        for (int i=0;i<scale;i++) {
            if (p.c!=img.pixels[y*img.width+x+i].c) {
                is_dot = false;
            }
        }
        if (is_dot) {
            for (int i=0;i<scale;i++) {
                if (p.c!=img.pixels[(y+1)*img.width+int(x+scale/2)].c) {
                    is_dot = false;
                }
            }
        }
        if (is_dot) {
            Dot dot;
            dot.x = x;
            dot.y = y;
            dot.c = p.c;
            dot.size_x = scale;
            dot.size_y = scale;
            dots.push_back(dot);
        }
    }
}
}

float count_thickness_rate(Image original, Image scaled, int img_scales) {
    std::vector<Line> original_lines;
    std::vector<Line> scaled_lines;

    float rate =1;
    int mistakes=0;

    count_diagonal_lines(original, original_lines);
    count_diagonal_lines(scaled, scaled_lines);

    //sprawdz czy ilosc lini sie zgadza
    if (original_lines.size()!=scaled_lines.size()) {return 0;}

    for (int i=0;i<original_lines.size();i++) {
        if (original_lines[i].thickness_x[i]*img_scales!=scaled_lines[i].thickness_x[i]) {mistakes++;}
    }
rate= rate/mistakes;

    std::vector<Dot> original_dots;
    std::vector<Dot> scaled_dots;
    find_dots_original(original, original_dots);
    find_dots_scaled(scaled, scaled_dots,img_scales);
    rate = rate/(original_dots.size()/scaled_dots.size());
    return rate;
 }