#include "func.h"
#include <vector>

struct Image_lines {
    int up;
    int down;
    int intersecting;
};

Image_lines count_intersections(Image img) {
    std::vector<std::vector<bool> > visitedDown(
        img.width,
        std::vector<bool>(img.height, false));

    std::vector<std::vector<bool> > visitedUp(
        img.width,
        std::vector<bool>(img.height, false));

    std::vector<Line> lines;


    int intersections_count =0;
    for (int x = 1; x < img.width; x++) {
        for (int y = 1; y < img.height; y++) {
            Pixel p = img.pixels[y*img.width + x];
            p.x = x;
            p.y = y;
            color c = p.c;
            //dla każdego piksela sprawdź czy jest początkiem

            if (!visitedDown[x][y]) {

                Line l = count_line(img, p, 1, 1, visitedDown);
                if (l.pixels.size() >= MIN_LINE_LENGTH) {
                    lines.push_back(l);
                }
            }
            if (!visitedUp[x][y]) {

                Line l = count_line(img, p, -1, 1, visitedUp);
                if (l.pixels.size() >= MIN_LINE_LENGTH) {
                    lines.push_back(l);
                }
            }
        if (visitedDown[x][y]) {
            if (visitedUp[x][y]) {
                ///debug only
                if (img == img_before_scaling) {intersections_original.push_back(p);}
                else {intersections_scaled.push_back(p);}


                intersections_count++;
        }
        }
            //mamy liczbe lini w dol i w gore i ich czesc wspolna. jak policzyc ile jesli lini w dol i w gore bez czesci wspolnej
            int down_count = visitedDown.size() - intersections_count;
            int up_count = visitedUp.size() - intersections_count;
            logFile<<"down: "<<down_count<<" up: "<<up_count<<" intersections: "<<intersections_count<<std::endl;
        return Image_lines(up_count,down_count, intersections_count);
    }
}
}

float count_intersections_rate(Image original, Image scaled, int scale) {
    logFile<<"ORIGINAL ";
    Image_lines original_lines = count_intersections(original);
    logFile<<"SCLAED ";
    Image_lines scaled_lines = count_intersections(scaled);
    float rate_up = original_lines.up*scale/scaled_lines.up;
    float rate_down = original_lines.down*scale/scaled_lines.down;
    float rate_intersections = original_lines.intersecting/scaled_lines.intersecting;
    logFile<<"rate_up: "<<rate_up<<" rate_down: "<<rate_down<<" intersections: "<<rate_intersections<<std::endl;
    return normalize((rate_up+rate_down+rate_intersections)/3,0,1);
}