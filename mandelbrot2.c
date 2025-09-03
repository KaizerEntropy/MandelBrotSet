#include <stdio.h>
#include <SDL2/SDL.h>
#include <complex.h>
#include <math.h>

// Window size
#define WIDTH 1500
#define HEIGHT 1000
#define SET_LIMIT 500   // more iterations = more detail

// Mandelbrot escape-time iteration
int mandelbrotCount(double real, double imag) {
    double _Complex n = real + imag*I;
    double _Complex z = 0;
    for (int i = 0; i < SET_LIMIT; i++) {
        z = z*z + n;
        if (cabs(z) > 2.0) return i;
    }
    return SET_LIMIT;
}

// Colour function
Uint32 pixelColour(SDL_Surface *surface, int iter) {
    int red, green, blue;
    if (iter == SET_LIMIT) {
        red = green = blue = 0;
    } else {
        double grad = (double)iter / SET_LIMIT;
        grad = pow(grad, 0.58);
        red = (int)(255 * grad);
        green = (int)(55 * grad);
        blue = (int)(55 * grad);
    }
    return SDL_MapRGB(surface->format, red, green, blue);
}

// Fill one pixel in target surface
void fillPixel(SDL_Surface *surface, int x, int y,
               double centerX, double centerY, double scale) {
    double real = centerX + (x - WIDTH / 2.0) * (scale / WIDTH);
    double imag = centerY + (y - HEIGHT / 2.0) * (scale / WIDTH);
    int count = mandelbrotCount(real, imag);
    Uint32 colour = pixelColour(surface, count);

    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = colour;
}

// Render a quick low-res Mandelbrot (skip pixels)
void renderQuick(SDL_Surface *canvas, double centerX, double centerY, double scale) {
    int step = 4; // skip 4 pixels for speed
    for (int y = 0; y < HEIGHT; y += step) {
        for (int x = 0; x < WIDTH; x += step) {
            fillPixel(canvas, x, y, centerX, centerY, scale);
        }
    }
}

// Render progressively row by row
int renderRow(SDL_Surface *canvas, int row, double centerX, double centerY, double scale) {
    if (row >= HEIGHT) return 0;
    for (int x = 0; x < WIDTH; x++) {
        fillPixel(canvas, x, row, centerX, centerY, scale);
    }
    return 1; // still rows left
}

// ------------------- MAIN -------------------
int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Hybrid Mandelbrot",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    // Persistent canvas
    SDL_Surface *canvas = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    // Mandelbrot view parameters
    double scale = 3.0;
    double centerX = -0.5, centerY = 0.0;

    int running = 1;
    SDL_Event event;

    // Progressive render state
    int row = 0;
    int refining = 0; // whether we are refining after zoom

    // First quick render
    renderQuick(canvas, centerX, centerY, scale);

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = 0;
                    }
                    else if (event.key.keysym.sym == SDLK_z) { // zoom in
                        scale *= 0.5;
                        renderQuick(canvas, centerX, centerY, scale);
                        row = 0; refining = 1;
                    }
                    else if (event.key.keysym.sym == SDLK_x) { // zoom out
                        scale *= 2.0;
                        renderQuick(canvas, centerX, centerY, scale);
                        row = 0; refining = 1;
                    }
                    else if (event.key.keysym.sym == SDLK_UP) {
                        centerY -= scale * 0.1;
                        renderQuick(canvas, centerX, centerY, scale);
                        row = 0; refining = 1;
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN) {
                        centerY += scale * 0.1;
                        renderQuick(canvas, centerX, centerY, scale);
                        row = 0; refining = 1;
                    }
                    else if (event.key.keysym.sym == SDLK_LEFT) {
                        centerX -= scale * 0.1;
                        renderQuick(canvas, centerX, centerY, scale);
                        row = 0; refining = 1;
                    }
                    else if (event.key.keysym.sym == SDLK_RIGHT) {
                        centerX += scale * 0.1;
                        renderQuick(canvas, centerX, centerY, scale);
                        row = 0; refining = 1;
                    }
                    break;

                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                        event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        surface = SDL_GetWindowSurface(window);
                    }
                    break;
            }
        }

        // If refining, draw one row per loop
        if (refining) {
            if (!renderRow(canvas, row, centerX, centerY, scale)) {
                refining = 0; // finished
            }
            row++;
        }

        // Blit canvas → window
        SDL_BlitSurface(canvas, NULL, surface, NULL);
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(canvas);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
