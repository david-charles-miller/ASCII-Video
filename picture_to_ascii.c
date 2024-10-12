#include <SDL2/SDL.h>        
#include <stdbool.h> 
#include <stdio.h>

int main(int argc, char ** argv)
{
    bool quit = false;
    SDL_Event event;
 
    SDL_Init(SDL_INIT_VIDEO);
 
    SDL_Window * window = SDL_CreateWindow("SDL2 Displaying Image",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
 
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface * image = SDL_LoadBMP("./Land.BMP");
	if(!image) {
		printf("Error: Image unable to load\n");
		return 1;
	}
	
	if(!image->pixels) {
		printf("Error: Image contains no pixels\n");
		return 1;
	
	}

	// Get the pixel format to use in extracting the RGB values from the image
	SDL_PixelFormat* pfmt = image->format;
	// Variables to hold the RGB values
	char red = 0;
	char green = 0;
	char blue = 0;
	int current_pixel = 0;
	int pitch = image->pitch;

	// File to dump pixel values to
	FILE* file;
	file = fopen("pixel_dump.txt", "w");
	if(!file) {
		printf("Error: Unable to create new file\n");
	}
	
	printf("SDL Surface struct :\n");
	printf("Pitch: %d\n", pitch);

	for(int r=0; r<image->h; r++) {
		for(int c=0; c<image->w; c++) {
			current_pixel = ((int*)(image->pixels))[r*image->pitch + c];
			SDL_GetRGB(current_pixel, pfmt, &red, &green, &blue);
			fprintf(file, "%d, %d, %d\n", red, green, blue);
		}
	}

	fclose(file);


	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);
 
    while (!quit)
    {
        SDL_WaitEvent(&event);
 
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
        }
 
        //SDL_Rect dstrect = { 5, 5, 320, 240 };
        //SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
 
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
 
    SDL_Quit();
 
    return 0;
}


