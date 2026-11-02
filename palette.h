#include <iostream>
#include <vector>
#include "func.h"
#include <algorithm>
#include <map>

///     WSPÓŁCZYNNIK ZGODNOŚCI PALETY KOLORYSTYCZNEJ
///

//mapuje wystapienie ile razy kazdy z kolorow wystapil w obrazie
std::map<color, int> get_color_counts(const Image& img) {
    std::map<color, int> counts;
    for (const auto& p : img.pixels) {
        counts[p.c]++;
    }
    return counts;
}

//obie palety są sortowane rosnąco według r, g i b
float check_same_size(std::vector<color> original_palette, std::vector<color> new_palette) {
    std::sort(original_palette.begin(), original_palette.end());
    std::sort(new_palette.begin(), new_palette.end());
    if (new_palette.size()==original_palette.size()) {
        for (int i=0; i<new_palette.size(); i++) {
            //sprawdź zawartość nowej palety i oblicz różnicę
            if (!check_same_color(original_palette[i], new_palette[i])) {
                p_difference++;
                break;
                }
        }
        // oblicz proporcję różniących się kolorów w stosunku do całej liczby kolorów
        int s = new_palette.size();
        p_difference = s - p_difference; // tyle kolorów jest takie samo
        p_rate /= p_difference;
        return p_rate;
    }
    else return 0.0f;
}

float check_different_size(std::vector<color> original_palette, std::vector<color> new_palette) {
        //sprawdź czy powiększenie doało nowe kolory (blur)
        if (new_palette.size()>original_palette.size()) {
            // policz ile kolorów zostało dodanych
            int new_colors = new_palette.size() - original_palette.size();
            //odejmij dodane kolory z nowej palety
            std::vector<color> filtered_pal;

            for (int i=0; i<new_palette.size(); i++) {

                for (int j=0; j<original_palette.size(); j++) {
                    if (check_same_color(new_palette[i], original_palette[j])) {
                        filtered_pal.push_back(new_palette[i]);
                        break;
                    }
                }
            }
            if (filtered_pal.size() == original_palette.size()) {
                return check_same_size(original_palette, filtered_pal);
            } else {
                return static_cast<float>(original_palette.size()) / static_cast<float>(new_palette.size());
            }
        }
        //powiększenie zmniejszyło liczbę kolorów - wszystkie nie zostały wykorzystane i jakość obrazu jest zmniejszona
        else {
            p_rate = 0;
            return p_rate;
        }

}

std::vector<int> check_color_encounters(std::vector<color> palette, Image img){
    std::vector<int>  encounters;
    for (int i=0; i<palette.size(); i++) {
        encounters.emplace_back(0);
    }
    //zakladam ze palety sa posortowane
    for (auto p : img.pixels) {
        for (int i=0; i<palette.size(); i++) {
            if (p.c == palette[i]) {
                encounters[i]++;
            }
        }
    }
    return encounters;
}

float check_palette(Image original_image, Image scaled_image, int scale) {

    if (new_palette.size()!=original_palette.size()) {
        return check_different_size(original_palette, new_palette);
    }

    std::map<color,int> original_encounters = get_color_counts(original_image);
    std::map<color,int> scaled_encounters = get_color_counts(scaled_image);

    float temp_proportion_rate = 0.0f;
    int color_count = 0;

    for (const auto& [col, orig_count] : original_encounters) {
        int expected_scaled_count = orig_count * scale * scale;
        int actual_scaled_count = scaled_encounters[col]; // jesli koloru nie ma= 0

        if (expected_scaled_count > 0) {
            //stosunek liczby pikseli do oczekiwanej w powiekszonym obrazie
            float ratio = static_cast<float>(actual_scaled_count) / static_cast<float>(expected_scaled_count);

            //ocena bliskości proporcji
            float deviation = std::abs(1.0f - ratio);
            float score = std::max(0.0f, 1.0f - deviation);

            temp_proportion_rate += score;
            color_count++;
        }
    }

    if (color_count == 0) return 0.0f;

    //float total_proportion_rate = temp_proportion_rate / static_cast<float>(color_count); -> dzielenie przez color count jest wewnatrz check same size i chcemy uniknac dzielenia przez kwadrat dolor count

    float base_palette_rate = check_same_size(original_palette, new_palette);

    return base_palette_rate * temp_proportion_rate;

}