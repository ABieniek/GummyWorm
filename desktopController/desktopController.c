#include "SDL/include/SDL.h"
#include "SDL/SDL2_image-2.0.2/SDL_image.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include "tcpserver.h"
#include <time.h>

typedef struct{
	int writebuffidx;
	char** filenameLists[2];
	int nameListLengths[2];
	pthread_mutex_t locks[2];
} displayArg;
void* runDisplay(void* arg);

int main(int argc, char **argv)
{
	serverArg* sa = malloc(sizeof(serverArg));
	sa->portnumber = 2410;
	sa->videoFileName = "gotvideo.webm";
	pthread_t serverid;
	pthread_create(&serverid, NULL, runServer, sa);

	displayArg* da = malloc(sizeof(displayArg));
	da->nameListLengths[0] = 16;
	da->filenameLists[0] = malloc(16*sizeof(char*));
	da->filenameLists[0][0] = "imagelist0/frame1.jpg";
	da->filenameLists[0][1] = "imagelist0/frame2.jpg";
	da->filenameLists[0][2] = "imagelist0/frame3.jpg";
	da->filenameLists[0][3] = "imagelist0/frame4.jpg";
	da->filenameLists[0][4] = "imagelist0/frame5.jpg";
	da->filenameLists[0][5] = "imagelist0/frame6.jpg";
	da->filenameLists[0][6] = "imagelist0/frame7.jpg";
	da->filenameLists[0][7] = "imagelist0/frame8.jpg";
	da->filenameLists[0][8] = "imagelist0/frame9.jpg";
	da->filenameLists[0][9] = "imagelist0/frame10.jpg";
	da->filenameLists[0][10] = "imagelist0/frame11.jpg";
	da->filenameLists[0][11] = "imagelist0/frame12.jpg";
	da->filenameLists[0][12] = "imagelist0/frame13.jpg";
	da->filenameLists[0][13] = "imagelist0/frame14.jpg";
	da->filenameLists[0][14] = "imagelist0/frame15.jpg";
	da->filenameLists[0][15] = "imagelist0/frame16.jpg";
	da->nameListLengths[1] = 17;
	da->filenameLists[1] = malloc(17*sizeof(char*));
	da->filenameLists[1][0] = "imagelist1/frame1.jpg";
	da->filenameLists[1][1] = "imagelist1/frame2.jpg";
	da->filenameLists[1][2] = "imagelist1/frame3.jpg";
	da->filenameLists[1][3] = "imagelist1/frame4.jpg";
	da->filenameLists[1][4] = "imagelist1/frame5.jpg";
	da->filenameLists[1][5] = "imagelist1/frame6.jpg";
	da->filenameLists[1][6] = "imagelist1/frame7.jpg";
	da->filenameLists[1][7] = "imagelist1/frame8.jpg";
	da->filenameLists[1][8] = "imagelist1/frame9.jpg";
	da->filenameLists[1][9] = "imagelist1/frame10.jpg";
	da->filenameLists[1][10] = "imagelist1/frame11.jpg";
	da->filenameLists[1][11] = "imagelist1/frame12.jpg";
	da->filenameLists[1][12] = "imagelist1/frame13.jpg";
	da->filenameLists[1][13] = "imagelist1/frame14.jpg";
	da->filenameLists[1][14] = "imagelist1/frame15.jpg";
	da->filenameLists[1][15] = "imagelist1/frame16.jpg";
	da->filenameLists[1][16] = "imagelist1/frame17.jpg";
	pthread_t displayid;
	pthread_create(&displayid, NULL, runDisplay, da);
	pthread_exit(NULL);
	exit(0);
}

void* runDisplay(void* arg)
{
	displayArg* da = (displayArg*) arg;
	SDL_Event event;
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG);

	SDL_Window * window = SDL_CreateWindow("SDL2 Displaying Image",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
		// find separate
	SDL_Surface * image;// = IMG_Load("image.jpg");
	SDL_Texture * texture;// = SDL_CreateTextureFromSurface(renderer, image);

	// I want to show the n images evenly over 1 second
	struct timespec screendur;
	bool quit = false;
	int buffidx = 0;
	while (!quit)
	{
		/*if (da->writebuffidx == 1) // we're gonna read from buffer 0
			buffidx = 0;
		else if (da->writebuffidx == 0) // we're gonna read from buffer 1
			buffidx = 1;*/
		screendur.tv_nsec = 1000000000 / da->nameListLengths[buffidx];
		int imgidx;
		for (imgidx = 0; imgidx < da->nameListLengths[buffidx]; imgidx++)
		{
			image = IMG_Load(da->filenameLists[buffidx][imgidx]);
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
			nanosleep(&screendur, NULL);
		}
		buffidx = (buffidx+1)%2;
	}

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	 	IMG_Quit();
	SDL_Quit();

	return 0;
}
