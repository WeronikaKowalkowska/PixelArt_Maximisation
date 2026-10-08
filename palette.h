#include <iostream>
#include <vector>
#include "func.h"

///     WSPÓŁCZYNNIK ZGODNOŚCI PALETY KOLORYSTYCZNEJ

//obie palety są sortowane rosnąco według r, g i b
float check_same_size(std::vector<color> original_palette, std::vector<color> new_palette) {
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

float check_palette(Image original_image, Image scaled_image) {

    if (new_palette.size()!=original_palette.size()) {

        return check_different_size(original_palette, new_palette);
    }
    else {
        return check_same_size(original_palette, new_palette);
    }

}