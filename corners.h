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
            //if (mask[dx+1][dy+1]==-1) continue;

            //if (p.x+dx<0||p.x+dx>=img.width||p.y+dy<0||p.y+dy>=img.height) return 0;

            //if (img.pixels[(p.y+dy)*img.width + (p.x+dx)].c==c &&mask[dx+1][dy+1]==0) match=false;
            //if (img.pixels[(p.y+dy)*img.width + (p.x+dx)].c==c &&mask[dx+1][dy+1]!=1) continue;
            bool is_same_color = (img.pixels[(p.y + dy) * img.width + (p.x + dx)].c == c);
            int m_val = mask[dy + 1][dx + 1]; // [y][x]
            if (m_val == -1) continue;

            int nx = p.x + dx;
            int ny = p.y + dy;
            if (nx < 0 || nx >= img.width || ny < 0 || ny >= img.height) return 0;

            bool same = (img.pixels[ny * img.width + nx].c == c);
            if (m_val == 1 && !same) return 0; // musi być ten sam kolor
            if (m_val == 0 && same)  return 0;}
    }
    if (match) return 1;
    else{ return 0;}
}

int scaled_matchMask(Image img, Pixel p, int startX, int startY, const int mask[6][6], color c) {
    bool match = true;
    for(int dy=-0; dy<6; dy++)
    {
        for(int dx=0; dx<6; dx++)
        {
            // +1 -> przesuniecie na indeksy tablicy [0,2]
            //if (mask[dx][dy]==-1) continue;

            //if (p.x+dx<0||p.x+dx>=img.width||p.y+dy<0||p.y+dy>=img.height) return 0;

            //if (img.pixels[(p.y+dy)*img.width + (p.x+dx)].c==c &&mask[dx+1][dy+1]==0) match=false;
            //if (img.pixels[(p.y+dy)*img.width + (p.x+dx)].c==c &&mask[dx+1][dy+1]!=1) continue;
            int m_val = mask[dy][dx]; // [y][x], bez +1!
            if (m_val == -1) continue;

            int nx = startX + dx;
            int ny = startY + dy;
            if (nx < 0 || nx >= img.width || ny < 0 || ny >= img.height) return 0;

            bool same = (img.pixels[ny * img.width + nx].c == c);
            if (m_val == 1 && !same) return 0; // musi być ten sam kolor
            if (m_val == 0 && same)  return 0; // musi być inny kolor
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

int count_scaled_convex(Image img) {
    int count = 0;
     for (int y = 0; y < img.height-6; y++){
        for (int x = 0; x < img.width-6; x++)
         {
            Pixel matrix[6][6];
            get_scaled_matrix(matrix, img, x, y);
            color c = img.pixels[(y + 2) * img.width + (x + 2)].c;
            for (int i =0;i<4;i++) {
                if (scaled_matchMask(img, matrix[0][0],x,y, scaled_convexMasks[i], c)==1) {
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

int count_scaled_concave(Image img) {
    int count = 0;
    for (int y = 0; y < img.height-6; y++){
        for (int x = 0; x < img.width-6; x++)
        {
            Pixel matrix[6][6];
            get_scaled_matrix(matrix, img, x, y);
            color c = img.pixels[(y + 2) * img.width + (x + 2)].c;
            for (int i =0;i<4;i++) {
                if (scaled_matchMask(img, matrix[0][0],x,y, scaled_concaveMasks[i], c)==1) {
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

float count_corner_quality_rate(Image original, Image scaled,int scale) {
    int original_concave = count_concave(original);
    int scaled_concave = count_scaled_concave(scaled);
    logFile<<"\nCONCAVE original"<<original_concave<<" scaled "<<scaled_concave;
    int original_convex = count_convex(original);
    int scaled_convex = count_scaled_convex(scaled);
    logFile<<"\nCONVEX original"<<original_convex<<" scaled "<<scaled_convex;

    int original_corners = original_concave+original_convex;
    //logFile<<"original corners: "<<original_corners<<std::endl;
    int scaled_corners = scaled_concave+scaled_convex;
    //logFile<<"scaled corners: "<<scaled_corners<<std::endl;
    if (original_corners == 0) return (scaled_corners == 0) ? 1.0f : 0.0f;

    //int expected_corners = original_corners*scale*scale;
    int expected_corners = original_corners;
    //logFile<<"expected corners: "<<expected_corners<<std::endl;
    //blad wzgledny
    float diff = std::abs(expected_corners - (float)scaled_corners) / expected_corners;
    //logFile<<"diff: "<<diff<<std::endl;
    float rate = 1.0f - diff ;
    //logFile<<"rate: "<<rate<<std::endl;
    return std::max(0.0f, std::min(1.0f, rate));
}



