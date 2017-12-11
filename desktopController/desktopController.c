#include "SDL/include/SDL.h"
#include "SDL/SDL2_image-2.0.2/SDL_image.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	bool quit = false;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG);

	SDL_Window * window = SDL_CreateWindow("SDL2 Displaying Image",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
		// find separate
	SDL_Surface * image;// = IMG_Load("image.jpg");
	SDL_Texture * texture;// = SDL_CreateTextureFromSurface(renderer, image);
	   
 
	while (!quit)
	{
		image = IMG_Load("image.jpg");
		texture = SDL_CreateTextureFromSurface(renderer, image);

	
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
	sleep(1);
	}

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	 	IMG_Quit();
	SDL_Quit();

	return 0;
}
