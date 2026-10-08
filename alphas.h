#include<vector>
#include <cmath>
#include "func.h"
#include "global.h"

///WSPOLCZYNNIK PIKSELI NIEPRZEZROCZYSTYCH

float count_alphas(Image original_image,Image scaled_image) {
    float rate = 1;
    for (int i=0;i<original_palette.size();i++) {
        original_alphas.push_back(original_palette[i].a);
    }
    for (int i=0;i<new_palette.size();i++) {
        new_alphas.push_back(new_palette[i].a);
    }

    int original_count = 0;
    float original_min = 1;
    float original_max = 0.1;
    for (int i = 0;i<original_alphas.size();i++) {
        if (original_alphas[i] != 1) {
            original_count++;
            if (original_alphas[i] <original_min) {
                original_min = original_alphas[i];
            }
            if (original_alphas[i] >original_max) {
                original_max = original_alphas[i];
            }
        }
        else {
            original_max = 1;
        }
    }
    float original_rate = original_alphas.size()/original_count; //taka jest liczba pikseli nieprzezroczystych w oryginalnej grafice
    normalize(original_rate, original_min, original_max);
    int new_count = 0;
    float new_min = 0.5;
    float new_max = 0.5;
    for (int i = 0;i<new_alphas.size();i++) {
        if (new_alphas[i] != 1) {
            new_count++;
            if (new_alphas[i] <new_min) {
                new_min = new_alphas[i];
            }
            if (new_alphas[i] >new_max) {
                new_max = new_alphas[i];
            }
        }
        else {
            new_max = 1;
        }
    }
    float new_rate = new_alphas.size()/new_count;
    normalize(new_rate, new_min, new_max);
    float difference = abs(new_rate - original_rate);
    return rate - difference;
}