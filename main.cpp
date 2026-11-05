#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>

#include "func.h"
#include "palette.h"
#include "alphas.h"
#include "diagonals.h"
#include "intersections.h"
#include "thickness.h"
#include "corners.h"
#include "debug_overlay .h"
#include "global.h"
#include "mmpx.h"

std::ofstream logFile;
std::ofstream outputFile;

bool init_window() {
    window = SDL_CreateWindow(
        "Quality Assesment",
        1920,
        1080,
        SDL_WINDOW_OPENGL
    );
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void render() {
    while (!window_done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                window_done = true;
            }
        }
        SDL_BlitSurface( surface_before_scaling, NULL, surface_resized, NULL );
        SDL_UpdateWindowSurface( window );
    }
}

MetricsResult calculate_metrics(Image& i1, Image& i2) {
    res.palette = check_palette(i1, i2, 2);
    res.alpha = count_alphas(i1, i2);
    res.diagonals = count_diagonals_rate(i1, i2, 2);
    res.intersections = count_intersections_rate(i1, i2, 2);
    res.thickness = count_thickness_rate(i1, i2, 2);
    res.corners = count_corner_quality_rate(i1, i2, 2);
    res.average = (res.palette + res.alpha + res.diagonals + res.intersections + res.thickness + res.corners) / 6.0f;
    return res;
}

std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void write_output(std::string scale_mode, MetricsResult& res, bool first) {
    if (!first) { outputFile << ",\n"; }
    outputFile << "{\n";
    outputFile << "     \"scale_mode\": \"" << scale_mode << "\",\n";
    outputFile << "         \"metrics\": {\n";
    outputFile << "         \"palette\": " << res.palette << ",\n";
    outputFile << "         \"transparency\": " << res.alpha << ",\n";
    outputFile << "         \"diagonals\": " << res.diagonals << ",\n";
    outputFile << "         \"intersections\": " << res.intersections << ",\n";
    outputFile << "         \"thickness\": " << res.thickness << ",\n";
    outputFile << "         \"corners\": " << res.corners << ",\n";
    outputFile << "         \"average\": " << res.average << "\n";
    outputFile << "  }\n";
    outputFile << "}\n";
}

Image surface_to_image(SDL_Surface *surf) {
    Image img;
    img.width = surf->w;
    img.height = surf->h;

    img.pixels.resize(img.width * img.height); //zapewnienie rozmiaru na korzystanie z mmpx

    SDL_LockSurface(surf);

    for(int y = 0; y < img.height; y++) {
        for(int x = 0; x < img.width; x++) {
            const SDL_Color c = getPixelColor(surf, x, y);
            const color p_color = {+c.r, +c.g, +c.b, (float)c.a};
            Pixel p = {x, y, p_color};
            img.pixels[y * img.width + x] = p;
        }
    }

    SDL_UnlockSurface(surf);

    return img;
}


void setup_surface(SDL_ScaleMode scale_mode) {
    logFile<<scale_mode<<"\n";
    if (surface_resized != NULL) {
        SDL_DestroySurface(surface_resized);
    }

    surface_resized = SDL_ScaleSurface(
        SDL_ConvertSurface(surface_before_scaling, SDL_PIXELFORMAT_RGBA32), //zapewnia zeby nie ignorowac flagi linear
        surface_before_scaling->w * 2,
        surface_before_scaling->h * 2,
        scale_mode
    );

    Image img_bilinear = surface_to_image(surface_resized);
    //logFile << scale_mode << " After scaling: w: " << img_bilinear.width << std::endl;
    //logFile << scale_mode << " After scaling: h: " << img_bilinear.height << std::endl;
    //logFile << scale_mode << " Pixels: " << img_bilinear.pixels.size() << std::endl;

}

void setup_palette_and_lines() {
    original_lines.clear();
    scaled_lines.clear();
    original_palette.clear();
    new_palette.clear();
    p_rate = 1;
    p_difference = 0;

    original_palette = getImagePalette(img_before_scaling);
    new_palette = getImagePalette(img_resized);

    count_diagonal_lines(img_before_scaling, original_lines);
    count_diagonal_lines(img_resized, scaled_lines);
}

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);

    logFile.open("../../log.json");
    outputFile.open("../../output.json");

    outputFile << "{\n"; outputFile << " \"timestamp\": \"" << get_current_timestamp() << "\",\n"; outputFile << " \"comparisons\": [\n";

    surface_before_scaling = SDL_LoadPNG( "../../Enlarger.png" );
    if( surface_before_scaling == NULL )
    {
        logFile << "Unable to load image! SDL Error: " << SDL_GetError() << "\n";
        window_done = false;
        return 1;
    }

    img_before_scaling = surface_to_image(surface_before_scaling);

    //logFile << "Before scaling: w: " << img_before_scaling.width << std::endl;
    //logFile << "Before scaling: h: " << img_before_scaling.height << std::endl;
    //logFile << "Pixels: " << img_before_scaling.pixels.size() << std::endl;

    setup_surface(SDL_SCALEMODE_LINEAR);
    img_resized = surface_to_image(surface_resized);
    setup_palette_and_lines();
    //SDL_SavePNG(surface_resized,"bilinear.png" );
    res = calculate_metrics(img_before_scaling, img_resized);
    write_output("bilinear", res,true);

    debug_lines(img_before_scaling, original_lines,"lines_original.png");
    debug_lines(img_resized, scaled_lines,"lines_resized.png");
    debug_corners(img_before_scaling, convex_original, concave_original, "corners_original.png");
    debug_corners(img_resized, convex_scaled, concave_scaled, "corners_resized.png");
    debug_intersections(img_before_scaling, intersections_original, "intersections_original.png");
    debug_intersections(img_resized, intersections_scaled, "intersections_scaled.png");

    setup_surface(SDL_SCALEMODE_NEAREST);
    img_resized = surface_to_image(surface_resized);
    setup_palette_and_lines();
    //SDL_SavePNG(surface_resized, "nearest.png");
    res = calculate_metrics(img_before_scaling, img_resized);
    write_output("nearest", res,false);

    logFile<<"MMPX\n";
    mmpx_surface = scale_mmpx(2,mmpx_img);
    img_resized = mmpx_img;
    setup_palette_and_lines();
    //SDL_SavePNG(mmpx_surface,"mmpx.png" );
    res = calculate_metrics(img_before_scaling, img_resized);
    write_output("mmpx", res,true);


    logFile.close();
    outputFile.close();

    SDL_DestroyWindow(window);

    window = NULL;

    SDL_Quit();
    return 0;
}