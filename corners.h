#include <format>
#include <vector>
#include "func.h"
#include "global.h"
///WSPÓŁCZYNNIK ZACHOWANIA NAROŻNIKÓW

int matchMask(Image& img, Pixel p, const int mask[3][3], color c)
{
    bool match = true;
    for(int dy=-1; dy<=1; dy++)
    {
        for(int dx=-1; dx<=1; dx++)
        {
            // +1 -> przesuniecie na indeksy tablicy [0,2]
            if (mask[dx+1][dy]==-1+1) continue;

            if (p.x+dx<0||p.x+dx>=img.width||p.y+dy<0||p.y+dy>=img.height) return 0;

            if (img.pixels[(p.y+dy)*img.width + (p.x+dx)].c==c &&mask[dx][dy]==0) match=false;
            if (img.pixels[(p.y+dy)*img.width + (p.x+dx)].c==c &&mask[dx][dy]==1) continue;
        }
    }
    if (match) return 1;
    else{ return 0;}
}


//wypukłe
int count_convex(Image img) {
    int count = 0;
    for (int x = 1; x < img.width-1; x++) {
        for (int y = 1; y < img.height-1; y++) {
            Pixel matrix[3][3];
            get_matrix(matrix, img, x, y);
            color c = matrix[1][1].c;
            for (int i =0;i<4;i++) {
                if (matchMask(img, matrix[1][1], convexMasks[i], c)==1) {
                    count++;
                    ///debug only
                    if (img == img_before_scaling) convex_original.push_back(img.pixels[y*img.width+(x)]);
                    else convex_scaled.push_back(img.pixels[y*img.width+(x)]);
                }
            }
        }
    }
    return count;
}



//wklęsłe
int count_concave(Image img) {
    int count = 0;
    for (int x = 1; x < img.width-1; x++) {
        for (int y = 1; y < img.height-1; y++) {
            Pixel matrix[3][3];

            get_matrix(matrix, img, x, y);

            color c = matrix[1][1].c;
            for (int i =0;i<4;i++) {
                if (matchMask(img, matrix[1][1], concaveMasks[i], c)==1) {
                    count++;
                    ///debug only
                    if (img == img_before_scaling) concave_original.push_back(img.pixels[y*img.width+(x)]);
                    else concave_scaled.push_back(img.pixels[y*img.width+(x)]);
                }
            }
        }
    }
    return count;
}

float count_corner_quality_rate(Image original, Image scaled,int scale) {
    int original_concave = count_concave(original);
    int scaled_concave = count_concave(scaled);
    int original_convex = count_convex(original);
    int scaled_convex = count_convex(scaled);

    int original_corners = original_concave+original_convex;
    //logFile<<"original corners: "<<original_corners<<std::endl;
    int scaled_corners = scaled_concave+scaled_convex;
    //logFile<<"scaled corners: "<<scaled_corners<<std::endl;
    if (original_corners == 0) return (scaled_corners == 0) ? 1.0f : 0.0f;

    int expected_corners = original_corners*scale*scale;
    //logFile<<"expected corners: "<<expected_corners<<std::endl;
    //blad wzgledny
    float diff = std::abs(expected_corners - (float)scaled_corners) / expected_corners;
    //logFile<<"diff: "<<diff<<std::endl;
    float rate = 1.0f - diff ;
    //logFile<<"rate: "<<rate<<std::endl;
    return std::max(0.0f, std::min(1.0f, rate));
}



