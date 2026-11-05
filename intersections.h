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
    std::vector<Line> linesDown;
    std::vector<Line> linesUp;

    int intersections_count =0;
    for (int x = 1; x < img.width; x++) {
        for (int y = 1; y < img.height; y++) {

            if (!is_edge_pixel(img, x, y)) continue;

             Pixel p = img.pixels[y*img.width + x];
             p.x = x;
            p.y = y;
            //dla każdego piksela sprawdź czy jest początkiem

            if (!visitedDown[x][y]) {

                Line l = count_line(img, p, 1, 1, visitedDown);
                if (l.pixels.size() >= MIN_LINE_LENGTH) {
                    linesDown.push_back(l);
                }
            }
            if (!visitedUp[x][y]) {

                Line l = count_line(img, p, -1, 1, visitedUp);
                if (l.pixels.size() >= MIN_LINE_LENGTH) {
                    linesUp.push_back(l);
                }
            }

        }
    }
    for (int x = 1; x < img.width; x++) {
        for (int y = 1; y < img.height; y++) {
            if (visitedDown[x][y]&&visitedUp[x][y]) {
                Pixel p = img.pixels[y*img.width + x];
                p.x = x;
                p.y = y;
                //color c = p.c;
                    ///debug only
                    if (img == img_before_scaling) {intersections_original.push_back(p);}
                    else {intersections_scaled.push_back(p);}


                intersections_count++;

            }
        }
}
    //mamy liczbe lini w dol i w gore i ich czesc wspolna. jak policzyc ile jesli lini w dol i w gore bez czesci wspolnej
    int down_count = linesDown.size();
    int up_count = linesUp.size();
    logFile<<"down: "<<down_count<<" up: "<<up_count<<" intersections: "<<intersections_count<<std::endl;
    return Image_lines(up_count,down_count, intersections_count);

}

float count_intersections_rate(Image original, Image scaled, int scale) {
    logFile<<"ORIGINAL ";
    Image_lines original_lines = count_intersections(original);
    logFile<<"SCLAED ";
    Image_lines scaled_lines = count_intersections(scaled);
    float rate_down = (scaled_lines.down > 0) ? std::min(1.0f, (float)scaled_lines.down / std::max(1.0f,(float)original_lines.down)) : 0.0f;
    float rate_up = (scaled_lines.up > 0) ? std::min(1.0f, (float)scaled_lines.up / std::max(1.0f, (float)original_lines.up)) : 0.0f;
    float rate_intersections = 1.0f;
    if (original_lines.intersecting > 0) {
        float diff = std::abs((float)scaled_lines.intersecting - original_lines.intersecting * scale);
        rate_intersections = std::max(0.0f, 1.0f - (diff / (original_lines.intersecting * scale)));
    } else if (scaled_lines.intersecting > 0) {
        std::cout <<"made 0.5\n";
        rate_intersections = 0.5f;
    }
    logFile<<"rate_up: "<<rate_up<<" rate_down: "<<rate_down<<" intersections: "<<rate_intersections<<std::endl;
    return (rate_up + rate_down + rate_intersections) / 3.0f;
}