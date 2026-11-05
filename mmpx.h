#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>

#include "global.h"

#include <iostream>

int luminance(color c) {
    return (c.r+c.g+c.b+1)*(256-static_cast<int>(c.a));
}

//sprawdzanie dopasowania kolorow
inline bool all_eq2(const color& B, const color& A0, const color& A1) {
    return B==A0 && B==A1;
}

inline bool all_eq3(const color& B, const color& A0, const color& A1, const color& A2) {
    return B==A0 && B==A1&&B==A2;
}

inline bool all_eq4(const color& B, const color& A0, const color& A1, const color& A2, const color& A3) {
    return B==A0 && B==A1&&B==A2&&B==A3;
}

inline bool any_eq3(const color& B, const color& A0, const color& A1, const color& A2) {
    return B==A0 || B==A1||B==A2;
}

inline bool none_eq2(const color& B, const color& A0, const color& A1) {
    return  B!=A0 && B!=A1;
}

inline bool none_eq4(const color& B, const color& A0, const color& A1, const color& A2, const color& A3) {
    return B!=A0 && B!=A1&&B!=A2&&B!=A3;
}





SDL_Surface *scale_mmpx(int scale, Image &mmpx_img) {
    //bezpiecznik bo mmpx mozna wykonywac tylko skalowanie x2
    if (scale != 2) scale = 2;

    SDL_Surface* mmpx_surface;

    mmpx_img.width= img_before_scaling.width*scale;
    mmpx_img.height= img_before_scaling.height*scale;

    mmpx_img.pixels.resize(mmpx_img.width * mmpx_img.height);

    //bezpiecznik pilnujacy zakresu aby nie wyjsc po za obszar wspolrzednych obrazu
    auto getSrc = [&](int px, int py) -> color{
        px = std::ranges::clamp(px, 0, img_before_scaling.width - 1);
        py = std::ranges::clamp(py, 0, img_before_scaling.height - 1);
        return img_before_scaling.pixels[py * img_before_scaling.width + px].c;
    };

    for (const Pixel P : img_before_scaling.pixels) {
        //pixele docelowe
        Pixel S1{P.x*scale,     P.y*scale,     P.c}; //LU
        Pixel S2{P.x*scale + 1, P.y*scale,     P.c};//RU
        Pixel S3{P.x*scale,     P.y*scale + 1, P.c};//LD
        Pixel S4{P.x*scale + 1, P.y + 1, P.c};//RD

        //najblizsze otoczenie pixela P
        color LU1 = getSrc(P.x - 1, P.y - 1);
        color U1  = getSrc(P.x,     P.y - 1);
        color RU1 = getSrc(P.x + 1, P.y - 1);
        color L1  = getSrc(P.x - 1, P.y);
        color R1  = getSrc(P.x + 1, P.y);
        color LD1 = getSrc(P.x - 1, P.y + 1);
        color D1  = getSrc(P.x,     P.y + 1);
        color RD1 = getSrc(P.x + 1, P.y + 1);

        //dalsze otoczenie piksela P
        color U2 = getSrc(P.x,     P.y - 2);
        color L2 = getSrc(P.x - 2, P.y);
        color R2 = getSrc(P.x + 2, P.y);
        color D2 = getSrc(P.x,     P.y + 2);

        bool has_equal = (LU1 == P.c) || (U1 == P.c) || (RU1 == P.c) ||
                          (L1 == P.c)  || (R1 == P.c) || (LD1 == P.c) ||
                          (D1 == P.c)  || (RD1 == P.c);

        //sprawdzanie czy wszystkie kolory otoczenia sie zgadzaja - jesli tak to algorytm wpada w fallback nearest aby skrocic obliczenia
        if (has_equal) {

            //oblicz luminesencje z U2, L2, R2 i D2
            int p_luma = luminance(P.c);
            int u1_luma = luminance(U1);
            int r1_luma = luminance(R1);
            int d1_luma = luminance(D1);
            int l1_luma = luminance(L1);

            //slop 1:1
            if ((L1 == U1) && !(L1 == D1) && !(L1 == R1) && (p_luma >= l1_luma || P.c == LU1) && any_eq3(P.c, LU1, RU1, LD1) && ((p_luma < l1_luma) || !(LU1 == L1) || !(P.c == U2) || !(P.c == L2))) {
                S1.c = L1;
            }
            if ((U1 == R1) && !(U1 == L1) && !(U1 == D1) && (p_luma >= u1_luma || P.c == RU1) && any_eq3(P.c, LU1, RU1, RD1) && ((p_luma < u1_luma) || !(RU1 == U1) || !(P.c == U2) || !(P.c == R2))) {
                S2.c = U1;
            }
            if ((D1 == L1) && !(D1 == R1) && !(D1 == U1) && (p_luma >= d1_luma || P.c == LD1) && any_eq3(P.c, LU1, LD1, RD1) && ((p_luma < d1_luma) || !(LD1 == D1) || !(P.c == D2) || !(P.c == L2))) {
                S3.c = D1;
            }
            if ((R1 == D1) && !(R1 == L1) && !(R1 == U1) && (p_luma >= r1_luma || P.c == RD1) && any_eq3(P.c, RU1, LD1, RD1) && ((p_luma < r1_luma) || !(RD1 == D1) || !(P.c == R2) || !(P.c == D2))) {
                S4.c = R1;
            }

            // intersections - sprawdzenie przeciec
            if (!(P.c == R1) && all_eq4(P.c, RU1, RD1, L1, L2) && all_eq2(R1, U1, D1) && !(R1 == getSrc(P.x + 3, P.y))) {
                S2.c = S4.c = R1;
            }
            if (!(P.c == L1) && all_eq4(P.c, LU1, LD1, R1, R2) && all_eq2(L1, U1, D1) && !(L1 == getSrc(P.x - 3, P.y))) {
                S1.c = S3.c = L1;
            }
            if (!(P.c == D1) && all_eq4(P.c, LD1, RD1, U1, U2) && all_eq2(D1, L1, R1) && !(D1 == getSrc(P.x, P.y + 3))) {
                S3.c = S4.c = D1;
            }
            if (!(P.c == U1) && all_eq4(P.c, LU1, RU1, D1, D2) && all_eq2(U1, L1, R1) && !(U1 == getSrc(P.x, P.y - 3))) {
                S1.c = S2.c = U1;
            }

            // triangle tips - sprawdzenie wierzcholkow
            if (u1_luma < p_luma && all_eq4(P.c, LD1, D1, RD1, D2) && none_eq4(P.c, LU1, L1, RU1, R1)) {
                S1.c = S2.c = U1;
            }
            if (d1_luma < p_luma && all_eq4(P.c, LU1, U1, RU1, U2) && none_eq4(P.c, L1, LD1, RD1, R1)) {
                S3.c = S4.c = D1;
            }
            if (r1_luma < p_luma && all_eq4(P.c, LU1, L1, LD1, L2) && none_eq4(P.c, U1, RU1, RD1, D1)) {
                S2.c = S4.c = R1;
            }
            if (l1_luma < p_luma && all_eq4(P.c, RU1, R1, RD1, R2) && none_eq4(P.c, U1, LU1, LD1, D1)) {
                S1.c = S3.c = L1;
            }

            // slop 2:1
            if (!(D1 == U1)) {
                if (!(D1 == LU1) && !(D1 == P.c) && !(D1 == RU1)) {
                    if (all_eq3(D1, LD1, R1, R2) && none_eq2(D1, L1, getSrc(P.x + 2, P.y - 1))) S3.c = S4.c;
                    if (all_eq3(D1, RD1, L1, L2) && none_eq2(D1, R1, getSrc(P.x - 2, P.y - 1))) S4.c = S3.c;
                }
                if (!(U1 == RD1) && !(U1 == LD1) && !(U1 == P.c)) {
                    if (all_eq3(U1, LU1, R1, R2) && none_eq2(U1, L1, getSrc(P.x + 2, P.y + 1))) S1.c = S2.c;
                    if (all_eq3(U1, RU1, L1, L2) && none_eq2(U1, R1, getSrc(P.x - 2, P.y + 1))) S2.c = S1.c;
                }
            }

            if (!(R1 == L1)) {
                if (!(L1 == RD1) && !(L1 == P.c) && !(L1 == RU1)) {
                    if (all_eq3(L1, LU1, D1, D2) && none_eq2(L1, U1, getSrc(P.x + 1, P.y + 2))) S1.c = S3.c;
                    if (all_eq3(L1, LD1, U1, U2) && none_eq2(L1, D1, getSrc(P.x + 1, P.y - 2))) S3.c = S1.c;
                }
                if (!(R1 == P.c) && !(R1 == LU1) && !(R1 == LD1)) {
                    if (all_eq3(R1, RU1, D1, D2) && none_eq2(R1, U1, getSrc(P.x - 1, P.y + 2))) S2.c = S4.c;
                    if (all_eq3(R1, RD1, U1, U2) && none_eq2(R1, D1, getSrc(P.x - 1, P.y - 2))) S4.c = S2.c;
                }
            }

        }

        //fallback nearest i zapisanie wartosci do struktury
        int outX = P.x * scale;
        int outY = P.y * scale;
        int outPitch = mmpx_img.width;

        mmpx_img.pixels[outY * outPitch + outX]             = S1;
        mmpx_img.pixels[outY * outPitch + (outX + 1)]       = S2;
        mmpx_img.pixels[(outY + 1) * outPitch + outX]       = S3;
        mmpx_img.pixels[(outY + 1) * outPitch + (outX + 1)] = S4;

    }
    return image_to_surface(mmpx_img);
}
