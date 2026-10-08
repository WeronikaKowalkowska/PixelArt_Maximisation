#include <format>
#include <vector>
#include "func.h"
///WSPÓŁCZYNNIK ZACHOWANIA NAROŻNIKÓW



const int convexMasks[4][3][3] =
{
    {
        {-1,-1,1},
        {-1,1,0},
        {1,0,-1}
    },
    {
            {1,-1,-1},
            {0,1,-1},
            {-1,0,1}
    },
    {
            {-1,0,1},
            {0,1,-1},
            {1,-1,-1}
    },
    {
            {1,0,-1},
            {-1,1,0},
            {-1,-1,1}
    }
};

const int concaveMasks[4][3][3] =
{
    {
        {-1,-1,0},
        {-1,0,1},
        {0,1,-1}
    },
    {
            {0,-1,-1},
            {1,0,-1},
            {-1,1,0}
    },
    {
            {-1,1,0},
            {1,0,-1},
            {0,-1,-1}
    },
    {
            {0,1,-1},
            {-1,0,1},
            {-1,-1,0}
    }
};

int matchMask(Image& img, Pixel p, const int mask[3][3], color c)
{
    bool match = true;
    for(int dy=-1; dy<=1; dy++)
    {
        for(int dx=-1; dx<=1; dx++)
        {
            //c==1
            // +1 -> przesuniecie na indeksy tablicy [0,2]
            if (mask[dx+1][dy]==-1+1) continue;

            if (p.x+dx<0||p.x+dx>=img.width||p.y+dy<0||p.y+dy>=img.height) return 0;

            if (img.pixels[(p.y+dy)*img.width + (p.x+dx)].c==c &&mask[dx][dy]==0) match=false;
               if (img.pixels[(p.y+dy)*img.width + (p.x+dx)].c==c &&mask[dx][dy]==1) continue;


        }
    }

    //return true;
    if (match) return 1;
    else{ return 0;}
}


//wypukłe
int count_convex(Image img) {
    int count = 0;
    for (int x = 1; x < img.width-1; x++) {
        for (int y = 1; y < img.height-1; y++) {
            Pixel matrix[3][3];
            //if (x!=0&&y!=0&&x<img.width&&y<img.height)
            get_matrix(matrix, img, x, y);
            //else break;

            color c = matrix[1][1].c;
            for (int i =0;i<4;i++) {
                if (matchMask(img, matrix[1][1], convexMasks[i], c)==1) count++;
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
                if (matchMask(img, matrix[1][1], concaveMasks[i], c)==1) count++;
            }
        }
    }
    return count;
}

float count_corner_quality_rate(Image original, Image scaled) {
    int original_concave = count_concave(original);
    int scaled_concave = count_concave(scaled);
    float concave_rate = (original_concave != 0) ? (float)scaled_concave / original_concave : 1.0f;
    int original_convex = count_convex(original);
    int scaled_convex = count_convex(scaled);
    float convex_rate = (original_convex != 0) ? (float)scaled_convex / original_convex : 1.0f;
    return (concave_rate+convex_rate)/2.0f;
}



