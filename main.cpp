#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_pixels.h>

#include "func.h"
#include "palette.h"
#include "alphas.h"
#include "diagonals.h"
#include "intersections.h"
#include "thickness.h"
#include"corners.h"

SDL_Color getPixelColor(SDL_Surface* surface, int x, int y) {
    Uint8 r, g, b,a;
    bool success = SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a);
    if (success) {
        SDL_Color c;
        c.r = r;
        c.g = g;
        c.b = b;
        c.a = a;

        return c;
    }
    else {
        std::cout<<( "Could not get pixel color \n" );
        return SDL_Color();
    }
}

bool validate_color (SDL_Color c) {
    if (c.r < 0 || c.r > 255 || c.g < 0 || c.g > 255 ||c.b < 0 || c.b > 255) {
        return true;
    }
        return false;
}

Image surface_to_image(SDL_Surface *surf) {
    Image img;
    img.width = surf->w;
    img.height = surf->h;

    std::vector<Pixel> img_pixels;

    //get pixels
    SDL_LockSurface(surf);

    for(int x=0; x<img.width; x++) {
        for(int y=0; y<img.height; y++) {
            const SDL_Color c = getPixelColor(surf, x, y);
            if (!validate_color(c)) {
            }
            //operatur + automatycznie konwertuje Uint8 (char) na int
            const color p_color = {+c.r,+c.g,+c.b};
            Pixel p = {x,y,p_color};
            img_pixels.push_back(p);
        }
    }
    img.pixels = img_pixels;

    SDL_UnlockSurface(surf);
    //SDL_FreeSurface(surf);

    return img;
}


//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

// /SDL_SCALEMODE_PIXELART

int main(int argc, char* argv[]) {

    SDL_Window *window;                    // Declare a pointer
    bool done = false;

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL3 window",                  // window title
        1920,                               // width, in pixels
        1080,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    else {
        gScreenSurface = SDL_GetWindowSurface( window );
    }

    gHelloWorld = SDL_LoadPNG( "/home/mochi/Documents/inzynierka/project/Enlarger.png" );
    if( gHelloWorld == NULL )
    {
        std::cout<<( "Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError() );
        done = false;
    }

    Image i1 = surface_to_image(gHelloWorld);
    // std::cout<<("Before scaling: w: ", i1.width)<<std::endl;
    // std::cout<<("Before scaling: h: ", i1.height)<<std::endl;
    // std::cout<<("Pixels: ", i1.pixels.size())<<std::endl;

//     bool nearest = SDL_BlitSurfaceScaled(
//     gHelloWorld, NULL,
//     gScreenSurface, NULL,
//     SDL_SCALEMODE_NEAREST
// );
    // if (nearest) {
    //     //std::cout<<( "Nearest image scaled\n" );
    // }
    // else {
    //     SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not scale image %s\n", SDL_GetError());
    // }
    gScreenSurface = SDL_ScaleSurface(
        // gHelloWorld,       // The source image surface
        // NULL,         // Rectangle defining the source area (or NULL for full)
        // gScreenSurface,       // The destination surface (e.g., window)
        // NULL,         // Rectangle defining the target size/position
        // SDL_SCALEMODE_LINEAR // Enables bilinear interpolation
        gHelloWorld,
        gHelloWorld->w * 2,
        gHelloWorld->h * 2,
        SDL_SCALEMODE_LINEAR
    );
    // if (gScreenSurface) {
    //     //std::cout<<( "Bilinear image scaled\n" );
    // }
    // else {
    //     SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not scale image %s\n", SDL_GetError());
    // }

    Image i2 = surface_to_image(gScreenSurface);
    // std::cout << "Before scaling: w: " << i1.width << std::endl;
    // std::cout << "Before scaling: h: " << i1.height << std::endl;
    // std::cout << "Pixels: " << i1.pixels.size() << std::endl;

    original_palette = getImagePalette(i1);
    new_palette = getImagePalette(i2);

    //Image img = surface_to_image(gScreenSurface);

    std::cout<<"BILINEAR\n";

    //Wypisanie z wymuszeniem wyczyszczenia bufora (std::endl lub std::flush):
    std::cout << "Kryterium palety: " << std::flush;
    float r1= check_palette(i1,i2);
    std::cout<<r1;

    std::cout<<"Kryterium przezroczystosci: "<<std::endl;
    float r2 = count_alphas(i1,i2);
    std::cout<<r2<<"\n";

    std::cout<< "Kryterium zachowania wygladzenia lini krzywych i ukosnych\n";
    float r3 = count_diagonals_rate(i1,i2,2);
    std::cout<<r3<<"\n";

    std::cout<<"Kryterium liczby przeciec\n";
    float r4 = count_intersections_rate(i1,i2,2);
    std::cout<<r4<<"\n";

    std::cout<<"Kryterium zachowania grubosci linii\n";
    float r5 = count_thickness_rate(i1,i2,2);
    std::cout<<r5<<"\n";

    std::cout<<"Kryterium zachowania naroznikow\n";
    float r6 = count_corner_quality_rate(i1,i2);
    std::cout<<r6<<"\n";

    std::cout<<"Srednia\n";
    float r7 = (r1+r2+r3+r4+r5+r6)/6.0f;
    std::cout<<r7<<"\n";

    std::cout<<std::endl<<std::endl;

    gScreenSurface = SDL_ScaleSurface(
    // gHelloWorld,       // The source image surface
    // NULL,         // Rectangle defining the source area (or NULL for full)
    // gScreenSurface,       // The destination surface (e.g., window)
    // NULL,         // Rectangle defining the target size/position
    // SDL_SCALEMODE_LINEAR // Enables bilinear interpolation
    gHelloWorld,
    gHelloWorld->w * 2,
    gHelloWorld->h * 2,
    SDL_SCALEMODE_NEAREST
);

std::cout<<"NEAREST\n";
    i2 = surface_to_image(gScreenSurface);
    new_palette = getImagePalette(i2);
    std::cout << "Kryterium palety: " << std::flush;
     r1= check_palette(i1,i2);
    std::cout<<r1;

    std::cout<<"Kryterium przezroczystosci: "<<std::endl;
     r2 = count_alphas(i1,i2);
    std::cout<<r2<<"\n";

    std::cout<< "Kryterium zachowania wygladzenia lini krzywych i ukosnych\n";
     r3 = count_diagonals_rate(i1,i2,2);
    std::cout<<r3<<"\n";

    std::cout<<"Kryterium liczby przeciec\n";
     r4 = count_intersections_rate(i1,i2,2);
    std::cout<<r4<<"\n";

    std::cout<<"Kryterium zachowania grubosci linii\n";
     r5 = count_thickness_rate(i1,i2,2);
    std::cout<<r5<<"\n";

    std::cout<<"Kryterium zachowania naroznikow\n";
     r6 = count_corner_quality_rate(i1,i2);
    std::cout<<r6<<"\n";

    std::cout<<"Srednia\n";
     r7 = (r1+r2+r3+r4+r5+r6)/6.0f;
    std::cout<<r7<<"\n";



    // while (!done) {
    //     SDL_Event event;
    //
    //     while (SDL_PollEvent(&event)) {
    //         if (event.type == SDL_EVENT_QUIT) {
    //             done = true;
    //         }
    //     }
    //     //Apply the image
    //     SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );
    //     // Do game logic, present a frame, etc.
    //
    //     //Update the surface
    //     SDL_UpdateWindowSurface( window );
    // }

    //Deallocate surface
    //SDL_FreeSurface( gHelloWorld );
    //gHelloWorld = NULL;

    // Close and destroy the window
    SDL_DestroyWindow(window);

    window = NULL;
    // Clean up
    SDL_Quit();
    return 0;
}