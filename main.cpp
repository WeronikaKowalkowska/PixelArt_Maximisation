#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>

#include "thirdparty/imgui-1.93.0/imgui.h"
//#include "thirdparty/imgui-1.93.0/imgui_impl_sdl3.h"

#include "func.h"
#include "palette.h"
#include "alphas.h"
#include "diagonals.h"
#include "intersections.h"
#include "thickness.h"
#include "corners.h"
#include "debug_overlay .h"
#include "global.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "mmpx.h"

std::ofstream logFile;
std::ofstream outputFile;
SDL_Renderer* renderer = nullptr;

bool init_window() {
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    if (main_scale <= 0.0f) main_scale = 1.0f;
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if (!SDL_CreateWindowAndRenderer("QualityAssesment", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags, &window, &renderer)) {
        printf("Error: SDL_CreateWindowAndRenderer(): %s\n", SDL_GetError());
        return false;
    }
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Setup Dear ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    //style.FontSizeBase = 16.0f;           // Default to 13.0f when unspecified, which allows crisp legacy font for non-4K screens.
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;

    // Setup scaling
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)


    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);


    return true;
}

void render() {
    float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    while (!window_done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event); // Przekazanie zdarzeń myszy/klawiatury do ImGui
            if (event.type == SDL_EVENT_QUIT) {
                window_done = true;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)) {
                window_done = true;
            }
        }
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        bool show = true;
        ImGui::ShowDemoWindow(&show);
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show)
        {
            ImGui::Begin("Another Window", &show);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColorFloat(renderer, clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
}

void close_window() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
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
}

void setup_palette_and_lines() {
    original_lines.clear();
    scaled_lines.clear();
    original_palette.clear();
    new_palette.clear();
    concave_original.clear();
    convex_original.clear();
    concave_scaled.clear();
    convex_scaled.clear();
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

    setup_surface(SDL_SCALEMODE_LINEAR);
    img_resized = surface_to_image(surface_resized);
    setup_palette_and_lines();
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
    res = calculate_metrics(img_before_scaling, img_resized);
    write_output("nearest", res,false);

    logFile<<"MMPX\n";
    mmpx_surface = scale_mmpx(2,mmpx_img);
    img_resized = mmpx_img;
    setup_palette_and_lines();
    res = calculate_metrics(img_before_scaling, img_resized);
    write_output("mmpx", res,true);


    logFile.close();
    outputFile.close();

    init_window();
    render();
    close_window();

    SDL_DestroyWindow(window);

    window = NULL;

    SDL_Quit();
    return 0;
}