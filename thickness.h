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

void count_line_thickness(Image img,std::vector<Line> &lines) {
    for (auto& l : lines) {
        int i = 0;
        l.thickness_x.clear();
        for ( auto& p : l.pixels ) {
            //l.thickness_x.push_back(1);
            int left =-1;
            int right =1;
            while (p.x + left >= 0&&img.pixels[p.y*img.width + p.x+left].c == p.c) {
                left--;
            }
            while (p.x + right < img.width&&img.pixels[p.y*img.width + p.x+right].c == p.c) {
                right++;
            }
            //l.thickness_x[i] = abs(left)+right;
            l.thickness_x.push_back(abs(left)+right);
            i++;
        }
    }
    }


float count_thickness_rate(Image original, Image scaled, int img_scales) {

    count_line_thickness(original, original_lines);
    count_line_thickness(scaled, scaled_lines);




    //float ratio = (float)scaled_lines.size() / (float)(original_lines.size());
    float total_orig_pixels = 0, total_scaled_pixels = 0;

    //zliczam piksele w liniach
    for(const auto& l : original_lines) for (int i =0;i< l.thickness_x.size();i++) total_orig_pixels += l.thickness_x[i];
    for(const auto& l : scaled_lines) for (int i =0;i< l.thickness_x.size();i++) total_scaled_pixels += l.thickness_x[i];

    float expected_pixels = total_orig_pixels * img_scales; //tyle pikseli w liniach oczekuje od skalowanego
    float pixel_score =  std::abs(expected_pixels - total_scaled_pixels) / expected_pixels; //wylicz stopien bledu na podstawie srednich dlugosci

    return std::exp(-pixel_score);


 }