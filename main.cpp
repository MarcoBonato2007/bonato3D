#include <SDL2/SDL.h>

#include <vector>
#include <iostream>
#include <chrono>

#include "headers/model.h"
#include "headers/drawing.h"
#include "headers/player.h"

void printInfo() {
    std::cout << "Width: " << width << "\n" << "Height: " << height << std::endl;
    std::cout << "Aspect: " << aspect << std::endl;
}

int main(int argc, char *argv[]) {
    std::string pathToModel = "";
    if (argc == 1) {
        std::cout << "Must enter a single argument: the path to the file you want to display" << std::endl;
        return 1;
    }
    else {
        pathToModel = argv[1];
    }

    width = 800;
    height = 600;
    aspect = (float)width/height;
    proj = {
        {1/tan(X_FOV/2), 0, 0, 0}, 
        {0, aspect/tan(X_FOV/2), 0, 0}, 
        {0, 0, (near+far)/(near-far), -1}, 
        {0, 0, (2*near*far)/(near-far), 0}
    };

    SDL_Window* window = SDL_CreateWindow("example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             width, height);

    frame_buffer = std::vector<uint32_t>(width*height); // see buffers.h
    depth_buffer = std::vector<float>(width*height);

    model = Model(); // no path provided -> standard cube model
    // model = Model(pathToModel); // see model.h
    
    printInfo();

    auto start = std::chrono::system_clock::now();
    int n = 0;

    bool running = true;
    SDL_Event ev;

    int cursor_x, cursor_y;
    int prev_cursor_x, prev_cursor_y;
    SDL_GetMouseState(&cursor_x, &cursor_y);
    SDL_GetMouseState(&prev_cursor_x, &prev_cursor_y);

    while (running) {
        SDL_GetMouseState(&cursor_x, &cursor_y);
        mouse_handler(cursor_x-prev_cursor_x, cursor_y-prev_cursor_y);
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {running = false;}
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {running = false;}
            if (ev.type == SDL_KEYDOWN) {keyboard_handler(ev);}
        }

        look_at = get_look_at(pos, pitch, yaw);
        mvp = proj*look_at;

        std::fill(frame_buffer.begin(), frame_buffer.end(), 0);
        std::fill(depth_buffer.begin(), depth_buffer.end(), -1.0f);

        model.draw();

        const void* pixel_data = depth_buffer.data();
        int pitch = width * sizeof(uint32_t); // bytes per row
        if (SDL_UpdateTexture(texture, nullptr, pixel_data, pitch) != 0) {
            std::fprintf(stderr, "SDL_UpdateTexture error: %s\n", SDL_GetError());
            break;
        }

        // Render the texture to the screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr); // stretches to window size if different
        SDL_RenderPresent(renderer);
        
        n++;
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << (float)n/elapsed_seconds.count() << std::endl;
    
    return 0;
}

// main is responsible for initializing glfw and starting a mainloop
// all glfw and opengl code is contained only inside here.

// TODO:

    // try rendering to a quad instead of using glDrawPixels
    // is it any faster??

    // Now try to remove all the glm use, and optimize math (barycentric interpolate depth and the left-ness thing)
    // Is it faster? good, keep it. Otherwise keep glm.
    // Now 

    // Then add more meshes / complicated stuff until it's slow.
    // Then work on basic optimizations:
        // automatically exclude backface triangles from drawing
        // Frustum culling
            // Check bounding boxes in world spaces
            // Check triangles in clip spaces
