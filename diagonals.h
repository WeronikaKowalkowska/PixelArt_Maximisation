#include <math.h>
#include <numeric>

#include "func.h"
#include <vector>

///WSPÓŁCZYNNIK LINI SKOŚNYCH

float count_diagonals_rate(Image original, Image scaled, int img_scales) {
    float rate =1;
    int mistakes=0;

    if (original_lines.empty() || scaled_lines.empty()) {std::cout<<"diagonals lines count 0"<<std::endl;return 0.0f;};

    //sprawdz czy ilosc lini sie zgadza
    float ratio = (float)scaled_lines.size() / (float)(original_lines.size());
    float total_orig_pixels = 0, total_scaled_pixels = 0;

    //zliczam piksele w liniach
    for(const auto& l : original_lines) total_orig_pixels += l.pixels.size();
    for(const auto& l : scaled_lines) total_scaled_pixels += l.pixels.size();

    float expected_pixels = total_orig_pixels * img_scales; //tyle pikseli w liniach oczekuje od skalowanego
    float pixel_score =  std::abs(expected_pixels - total_scaled_pixels) / expected_pixels; //wylicz stopien bledu na podstawie srednich dlugosci

    return std::exp(-pixel_score);

}




