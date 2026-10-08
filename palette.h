#include <iostream>
#include <vector>
#include "func.h"

///     WSPÓŁCZYNNIK ZGODNOŚCI PALETY KOLORYSTYCZNEJ



//obie palety są sortowane rosnąco według r, g i b

float p_rate = 1;
int p_difference = 0;


float check_same_size(std::vector<color> original_palette, std::vector<color> new_palette) {
    std::cout<<("checking same size\n");
    if (new_palette.size()==original_palette.size()) {
        for (int i=0; i<new_palette.size(); i++) {
            //sprawdź zawartość nowej palety i oblicz różnicę
            if (!check_same_color(original_palette[i], new_palette[i])) {
                p_difference++;
                std::cout<<p_difference<<std::endl;
                break;
                }
        }
        // oblicz proporcję różniących się kolorów w stosunku do całej liczby kolorów
        int s = new_palette.size();
        std::cout<<"s"<<s<<std::endl;
        std::cout<<"diff"<<p_difference<<std::endl;
        p_difference = s - p_difference; // tyle kolorów jest takie samo
        std::cout<<"diff-s"<<p_difference<<std::endl;
        p_rate /= p_difference;
        std::cout<<"rate"<<p_rate<<std::endl;
        return p_rate;
    }
    else return 0.0f;
}

float check_different_size(std::vector<color> original_palette, std::vector<color> new_palette) {
    std::cout<<("checking diff size\n");
        //sprawdź czy powiększenie doało nowe kolory (blur)
        if (new_palette.size()>original_palette.size()) {
            std::cout<<"Rate is not 0"<<std::endl;
            // policz ile kolorów zostało dodanych
            int new_colors = new_palette.size() - original_palette.size();
            std::cout<<"Diff colors: "<<new_colors<<std::endl;
            //odejmij dodane kolory z nowej palety

            // Filtrujemy nową paletę, zachowując tylko te kolory, które były w oryginalnej
            std::vector<color> filtered_pal;

            for (int i=0; i<new_palette.size(); i++) {

                for (int j=0; j<original_palette.size(); j++) {
                    if (check_same_color(new_palette[i], original_palette[j])) {
                        //difference++;
                        // new_colors--;
                        // //std::cout<<new_colors<<std::endl;
                        // new_palette.erase(new_palette.begin()+i);
                        // if (new_colors==0) {break;}
                        filtered_pal.push_back(new_palette[i]);
                        break;
                    }
                }

                //     if (new_colors==0) {break;}
                // }v
                // if (new_colors==0) {break;}
            }
            if (filtered_pal.size() == original_palette.size()) {
                return check_same_size(original_palette, filtered_pal);
            } else {
                return static_cast<float>(original_palette.size()) / static_cast<float>(new_palette.size());
            }
            // //sprawdź czy po odjęciu palety są takie same
            // if (new_colors==0) {
            //     return check_same_size(original_palette, new_palette);
            // }
            // else {
            //    return check_different_size(original_palette, new_palette);
            // }
        }
        //powiększenie zmniejszyło liczbę kolorów - wszystkie nie zostały wykorzystane i jakość obrazu jest zmniejszona
        else {
            std::cout<<"Rate is 0\n";
            p_rate = 0;
            return p_rate;
        }

}

float check_palette(Image original_image, Image scaled_image) {

    //std::vector<color> palette;



    if (new_palette.size()!=original_palette.size()) {

        return check_different_size(original_palette, new_palette);
    }
    else {

        std::cout<<original_palette.size()<<" "<<new_palette.size()<<std::endl;
        return check_same_size(original_palette, new_palette);
    }

}