#include <math.h>

#include "func.h"
#include "global.h"
#include <vector>

//wyszukaj kropki jako pojedyńcze pixele


void find_dots_original(Image img,std::vector<Dot> &dots) {
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

void find_dots_scaled(Image img,std::vector<Dot> &dots, int scale) {
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
    if (original_lines.empty()) return 0.0f;
    float rate =1;
    int mistakes=0;

    //sprawdz czy ilosc lini sie zgadza

    int total_thickness_checks = 0;
    int correct_thickness_checks = 0;

    for (const auto& l : original_lines) {
        // oblicz wspolrzedne punktu startowego po skalowaniu
        int scaled_x = l.startX * img_scales;
        int scaled_y = l.startY * img_scales;

        if (scaled_x >= scaled.width || scaled_y >= scaled.height) continue;

        color line_color = img_before_scaling.pixels[l.startY * img_before_scaling.width + l.startX].c;

        int l_thick = 0, r_thick = 0;

        //zmierz grubosc na lewo i na prawo
        while (scaled_x - l_thick >= 0 &&
               scaled.pixels[scaled_y * scaled.width + (scaled_x - l_thick)].c == line_color) {
            l_thick++;
               }
        while (scaled_x + r_thick < scaled.width &&
               scaled.pixels[scaled_y * scaled.width + (scaled_x + r_thick)].c == line_color) {
            r_thick++;
               }

        int measured_scaled_thickness = l_thick + r_thick - 1;
        int expected_thickness = (l.thickness_x.empty() ? 1 : l.thickness_x[0]) * img_scales;

        total_thickness_checks++;
        // margines bledu 1 piksel dla wygladzania bilinearnego
        if (std::abs(measured_scaled_thickness - expected_thickness) <= 1) {
            correct_thickness_checks++;
        }
    }

    return (total_thickness_checks > 0) ? (float)correct_thickness_checks / total_thickness_checks : 0.0f;


 }