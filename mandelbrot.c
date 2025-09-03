//The Devil's Mandelbrot:

#include <stdio.h>
#include <SDL2/SDL.h>
#include <complex.h>

//Defining all necessary macros
#define WIDTH 1500
#define HEIGHT 1000
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define SET_LIMIT 400

//Checking if a complex number lies inside the Mandelbrot set
int mandelbrotCount(double real, double imag){
    double _Complex n = real + imag*I;
    double _Complex z = 0;
    for (int i=0;i<SET_LIMIT;i++){
        z = cpow(z, 2) + n;
        if (cabs(z)>2.0){ //Checking the condition
            return i;
        }
    }
    return SET_LIMIT;
}

//Creating the ggradient
Uint32 pixelColour(SDL_Surface *surface, int iter){
    int red, green, blue;
    if (iter == SET_LIMIT){ //Setting the colour of the pixels lying inside the Mandelbrot set
        red = green = blue = 0; //Inside is black
    }

    //Setting the gradient shift effect 
    else {
        double grad = 1.0*iter/SET_LIMIT;
            grad = pow(grad, 0.58); //Hit and trial set-up of which power has best visual effect (according to me)
            red = (int)(255*grad);
            green = (int)(55*grad);
            blue = (int)(55*grad);
    }
    return SDL_MapRGB(surface->format, red, green, blue);
}

//Filling the pixels
void fillPixel(SDL_Surface *surface, int x, int y){
    SDL_Rect pixel = (SDL_Rect) {x, y, 1, 1};
    int count = mandelbrotCount((double) 3.0*(x-2*WIDTH/3)/WIDTH, (double) 2.0*(y-HEIGHT/2)/HEIGHT); //Setting my custom co-ordinates for best optics
    Uint32 colour = pixelColour(surface, count);
    SDL_FillRect(surface, &pixel, colour);
}

int main(){

    //Initializing the animation
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);    
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    //SDL_Surface *canvas = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    
    //Running the animation
    int sim_run = 1, y = 0;
    while(sim_run){
        if (y<HEIGHT){
            for (int x=0;x<WIDTH;x++){
                fillPixel(surface, x, y); //Filling up pixels row by row
            }
            y++;
        }
        SDL_Event event;
        while (SDL_PollEvent(&event)){ //Provisioning for quit event
            switch (event.type){
                case SDL_QUIT:
                    sim_run = 0;
                    break;
            }
        }
        SDL_UpdateWindowSurface(window);
    }

    //Quiting the program
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

/* Made by:
Kaizer_Entropy
(Diptarko Bhattacharjee)
(JU BCSE2 Batch of 2028)
*/