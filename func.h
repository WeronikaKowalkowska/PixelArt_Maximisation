#ifndef FUNC_H
#define FUNC_H

#include <numeric>
#include <vector>
#include <cmath>



struct color {
    int r, g, b;
    float a;

     bool operator==(const color &other) {
        return r == other.r && g == other.g && b == other.b;
    }
};

std::vector<color> original_palette;
std::vector<color> new_palette;

struct Pixel {
    int x;
    int y;
    color c;

     bool operator==(const Pixel &other) {
        return x == other.x && y == other.y&&c==other.c;
    }
};

struct Image {
    int width;
    int height;
    //Pixel pixels[];
    std::vector<Pixel> pixels;
};

struct Line {
    color c;

    int pixels = 0;
    int stairs;
    // double length;
    //int thickness_x[pixels] = {1};
    std::vector<int>thickness_x ;

    int startX;
    int startY;

    int endX;
    int endY;
};

static float normalize(float n, float n_min, float n_max) {
    return (n - n_min) / (n_max - n_min);
}

inline void get_matrix(Pixel matrix[3][3], Image img, int x, int y) {
    matrix[1][1] = img.pixels[y * img.width + x];
    matrix[0][0] = img.pixels[(y - 1) * img.width + x - 1];
    matrix[0][1] = img.pixels[(y) * img.width + x - 1];
    matrix[0][2] = img.pixels[(y + 1) * img.width + x - 1];
    matrix[1][0] = img.pixels[(y - 1) * img.width + x];
    matrix[1][2] = img.pixels[(y + 1) * img.width + x];
    matrix[2][0] = img.pixels[(y - 1) * img.width + x + 1];
    matrix[2][1] = img.pixels[(y) * img.width + x + 1];
    matrix[2][2] = img.pixels[(y + 1) * img.width + x + 1];
}

bool check_same_color(color c1, color c2) {
    if (c1.r == c2.r &&
    c1.g == c2.g &&
    c1.b == c2.b) {
        return true;
        //break; // <-- Wykonuje się TYLKO wtedy, gdy WSZYSTKIE 3 składowe są identyczne!
    }
    return false;
}

std::vector<color> getImagePalette(Image &img) {
    std::vector<color> palette;
    for (int i=0; i<img.pixels.size(); i++) {
        //if (i==0) palette.push_back(img.pixels[i].c);
        color img_c = img.pixels[i].c;
        bool is_in_palette = false;
        for (int j=0; j<palette.size(); j++) {
            if (check_same_color(img_c, palette[j])) {
                is_in_palette = true;
                break; // Przerywamy tylko wtedy, gdy kolor został znaleziony
            }
        }
        if (!is_in_palette) {
            palette.push_back(img_c);
        }
    }
    return palette;
}

// static bool is_start(Image img, Pixel p, float dir_x, float dir_y) {
//     //if (p.c == img.pixels[(dir_y) * img.width + (dir_x)].c) return true;
//     //if (p.c==img.pixels[(dir_y) * img.width + (dir_x)-1].c) return true;
//     //else return false;
//     if (p.x+dir_x<0||p.x+dir_x>=img.width||p.y<0||p.y>=img.height) return false;
//     return p.c==img.pixels[(p.y+dir_y) * img.width + p.x+dir_x].c;
// }

static bool is_start(Image img, Pixel p, float dir_x, float dir_y) {
    // DODANO sprawdzanie z dir_y!
    if (p.x + dir_x < 0 || p.x + dir_x >= img.width || p.y + dir_y < 0 || p.y + dir_y >= img.height) return false;
    return p.c == img.pixels[(int(p.y + dir_y)) * img.width + int(p.x + dir_x)].c;
}


//od gornego lewego rogu w dol
static float directions_down[3][2] =
{
    {1, 0},
    {0, 1},
    {1, 1}
};

//od dolnego lewego rogu w gore
static float directions_up[3][2] =
{
    {0, 1},
    {-1, 0},
    {-1, 1}
};


static Line count_line(Image &img, Pixel p1, Pixel p2, float dirs[3][2], std::vector<std::vector<bool> > &visited) {
    //int n = 0;
    int len = 2;
    Line l;
    l.startX = p1.x;
    l.startY = p1.y;
    if (p1.x < 0 || p1.x >= img.width || p1.y < 0 || p1.y >= img.height||p2.x < 0 || p2.x >= img.width || p2.y < 0 || p2.y >= img.height) {
        //std::cout<<"Zwracam pusty\n";
        return l;
    }
    while (p1.c == p2.c) {
        // if (p1.x >= img.width && p1.y >= img.height) {
        //     break;
        // }
        l.pixels++;
        //std::cout<<"incremented pixel\n";
        visited[p1.x][p1.y] = true;
        visited[p2.x][p2.y] = true;

        ///pomiar grubości w lewo
        ///
        //sprawdz grubosc na lewo
        int left = 1;
        int l_thickness = 0;
        // ZMIEŃ NA:
        while (p2.x - left >= 0 && p2.c == img.pixels[p2.y * img.width + p2.x - left].c) {

            visited[p2.x - left][p2.y] = true;
            left++;
            //if (l.thickness_x[l.pixels])
            //l.thickness_x[l.pixels] = l.thickness_x[l.pixels]+1; ///
            //l.thickness_x[l.pixels]=left;
            l_thickness++;

            //std::cout<<"checking left "<<left<<" for pixel "<<l.pixels<<" on position x "<<p2.x-left<<" y "<<p2.y<<"\n";

        }
        //sprawdz grubosc na prawo
        int right = 1;
        int r_thickness = 0;
        while (p2.x + right < img.width&&p2.c == img.pixels[p2.y * img.width + p2.x + right].c) {

            visited[p2.x + right][p2.y] = true;
            right++;
            //l.thickness_x[l.pixels]++;
            r_thickness++;

            //std::cout<<"checking right "<<right<<" for pixel "<<l.pixels<<" on position x "<<p2.x+right<<" y "<<p2.y<<"\n";
        }
        //dodawanie grubosci do wektora
        l.thickness_x.push_back(1+l_thickness+r_thickness);

        //szukaj kolejnego piksela lini
        bool has_next = false;

        for (int i = 0; i < 3; i++) {
            int next_x = p2.x+dirs[i][0];
            int next_y = p2.y+dirs[i][1];
            if (next_x>=0&&next_x<img.width&&next_y>=0&&next_y<img.height) {
                Pixel next_p = img.pixels[next_y * img.width + next_x];
                // NADPISZ współrzędne tymi, o których wiesz, że są poprawne:
                next_p.x = next_x;
                next_p.y = next_y;
                if (next_p.c == p2.c&& !visited[next_x][next_y]) {
                    p1=p2;
                    p2=next_p;
                    has_next = true;
                    break;
                }
            }
        }

        if (has_next) {
           // std::cout<<"found next\n";
            l.endX = p2.x;
            l.endY = p2.y;
            //oblicz przesuniecie lini
            int dx = abs(l.endX - l.startX);
            int dy = abs(l.endY - l.startY);
            l.stairs = std::gcd(dx, dy);

        }

        else {
          //  std::cout<<"returned line\n";
            return l;
        }


    }
    //std::cout<<"returned line\n";
                return l;
            }





//oblicz ilosc schodkow w zaleznosci od pozycji poczatkowej i koncowej

static void count_diagonal_lines(Image img, std::vector<Line> &lines)
{
    std::vector<std::vector<bool> > visitedDown(
        img.width,
        std::vector<bool>(img.height, false));

    std::vector<std::vector<bool> > visitedUp(
        img.width,
        std::vector<bool>(img.height, false));


    for (int x = 1; x < img.width; x++) {
        for (int y = 1; y < img.height; y++) {
            Pixel p = img.pixels[y * img.width + x];

            p.x = x;
            p.y = y;

            color c = p.c;
            //dla każdego piksela sprawdź czy jest początkiem
            if (!visitedDown[x][y]) {
                for (int i = 0; i < 3; i++) {
                    if (is_start(img, p, directions_down[i][0], directions_down[i][1])) {
                        Line l = count_line(
                            img, p,
                            img.pixels[(y + directions_down[i][1]) * img.width + (x + directions_down[i][0])],
                            directions_down,
                            visitedDown);
                        lines.push_back(l);
                    }
                }
            }
            if (!visitedUp[x][y]) {
                for (int i = 0; i < 3; i++) {
                    if (is_start(img, p, directions_up[i][0], directions_up[i][1])) {
                        Line l = count_line(
                            img, p, img.pixels[(y + directions_up[i][1]) * img.width + (x + directions_up[i][0])],
                            directions_up,
                            visitedUp);
                        lines.push_back(l);
                    }
                }
            }
        }
    }
}

#endif
