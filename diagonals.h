#include <math.h>
#include <numeric>

#include "func.h"
#include <vector>

///WSPÓŁCZYNNIK LINI SKOŚNYCH

 //
// //if point is 0.0

// //sprawdza czy kolor w danym kierunku sie zgadza


float count_diagonals_rate(Image original, Image scaled, int img_scales) {
    std::vector<Line> original_lines;
    std::vector<Line> scaled_lines;

    float rate =1;
    int mistakes=0;

    count_diagonal_lines(original, original_lines);
    count_diagonal_lines(scaled, scaled_lines);

    //sprawdz czy ilosc lini sie zgadza
    if (original_lines.size()!=scaled_lines.size()) {return 0;}


    for (int i=0;i<original_lines.size();i++) {
        //sprawdz czy dlugosc lini sie zgadza
        if (original_lines[i].pixels*img_scales!=scaled_lines[i].pixels) {
            mistakes++;
        }
        //sprawdz czy ilosc schodkow w liniach sie zgadza
        if (original_lines[i].stairs*img_scales!=scaled_lines[i].stairs) {
            mistakes++;
        }
    }
    if (mistakes==0) {return 1;}
    else return rate/mistakes;

}




