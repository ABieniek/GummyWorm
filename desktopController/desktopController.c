#include "SDL/include/SDL.h"
#include "SDL/SDL2_image-2.0.2/SDL_image.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
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
#include <dirent.h>

typedef struct{
	char** filenameLists[2];
	int nameListLengths[2];
	char* videoFileName;
	int writebuffidx;
	pthread_mutex_t* readlock;
	pthread_barrier_t* barrier;
	pthread_cond_t* writecond;
	pthread_mutex_t* writelock;
	bool* videoWritten;
	char* outdir[2];
	char* outdirstar[2];
	char* outname[2];
} displayArg;

void* runDisplay(void* arg);
void* runffmpeg(void* arg);

int main(int argc, char **argv)
{
	serverArg* sa = malloc(sizeof(serverArg));
	sa->portnumber = 2410;
	sa->videoFileName = "gotvideo.webm";
	 pthread_t serverid;
	pthread_create(&serverid, NULL, runServer, sa);

	displayArg* da = malloc(sizeof(displayArg));
	da->writecond = sa->writecond = malloc(sizeof(pthread_cond_t));
	da->writelock = sa->writelock = malloc(sizeof(pthread_mutex_t));
	da->readlock = malloc(sizeof(pthread_mutex_t));
	da->barrier = malloc(sizeof(pthread_barrier_t));
	da->videoWritten = sa->videoWritten = malloc(sizeof(bool));
	pthread_mutex_init(da->writelock, NULL);
	pthread_cond_init(da->writecond, NULL);
	pthread_mutex_init(da->readlock, NULL);
	pthread_barrier_init(da->barrier, NULL, 2);
	*(da->videoWritten) = false;
	da->videoFileName = sa->videoFileName = "gotvideo.webm";
	da->outdir[0] = "imagelist0/";
	da->outdir[1] = "imagelist1/";
	da->outdirstar[0] = "imagelist0/*";
	da->outdirstar[1] = "imagelist1/*";
	da->outname[0] = "imagelist0/frame%03d.jpg";
	da->outname[1] = "imagelist1/frame%03d.jpg";
	da->writebuffidx = 0;

	/*
	da->nameListLengths[0] = 17;
	da->filenameLists[0] = malloc(17*sizeof(char*));
	da->filenameLists[0][0] = "imagelist0/frame001.jpg";
	da->filenameLists[0][1] = "imagelist0/frame002.jpg";
	da->filenameLists[0][2] = "imagelist0/frame003.jpg";
	da->filenameLists[0][3] = "imagelist0/frame004.jpg";
	da->filenameLists[0][4] = "imagelist0/frame005.jpg";
	da->filenameLists[0][5] = "imagelist0/frame006.jpg";
	da->filenameLists[0][6] = "imagelist0/frame007.jpg";
	da->filenameLists[0][7] = "imagelist0/frame008.jpg";
	da->filenameLists[0][8] = "imagelist0/frame009.jpg";
	da->filenameLists[0][9] = "imagelist0/frame010.jpg";
	da->filenameLists[0][10] = "imagelist0/frame011.jpg";
	da->filenameLists[0][11] = "imagelist0/frame012.jpg";
	da->filenameLists[0][12] = "imagelist0/frame013.jpg";
	da->filenameLists[0][13] = "imagelist0/frame014.jpg";
	da->filenameLists[0][14] = "imagelist0/frame015.jpg";
	da->filenameLists[0][15] = "imagelist0/frame016.jpg";
	da->filenameLists[0][16] = "imagelist0/frame017.jpg";
	da->nameListLengths[1] = 18;
	da->filenameLists[1] = malloc(18*sizeof(char*));
	da->filenameLists[1][0] = "imagelist1/frame001.jpg";
	da->filenameLists[1][1] = "imagelist1/frame002.jpg";
	da->filenameLists[1][2] = "imagelist1/frame003.jpg";
	da->filenameLists[1][3] = "imagelist1/frame004.jpg";
	da->filenameLists[1][4] = "imagelist1/frame005.jpg";
	da->filenameLists[1][5] = "imagelist1/frame006.jpg";
	da->filenameLists[1][6] = "imagelist1/frame007.jpg";
	da->filenameLists[1][7] = "imagelist1/frame008.jpg";
	da->filenameLists[1][8] = "imagelist1/frame009.jpg";
	da->filenameLists[1][9] = "imagelist1/frame010.jpg";
	da->filenameLists[1][10] = "imagelist1/frame011.jpg";
	da->filenameLists[1][11] = "imagelist1/frame012.jpg";
	da->filenameLists[1][12] = "imagelist1/frame013.jpg";
	da->filenameLists[1][13] = "imagelist1/frame014.jpg";
	da->filenameLists[1][14] = "imagelist1/frame015.jpg";
	da->filenameLists[1][15] = "imagelist1/frame016.jpg";
	da->filenameLists[1][16] = "imagelist1/frame017.jpg";
	da->filenameLists[1][17] = "imagelist1/frame018.jpg";
	*/

	pthread_t ffmpegid;
	pthread_create(&ffmpegid, NULL, runffmpeg, da);
	pthread_t displayid;
	pthread_create(&displayid, NULL, runDisplay, da);
	pthread_exit(NULL);
	exit(0);
}

void* runffmpeg(void* arg)
{
	while (1)
	{
		displayArg* da = (displayArg*) arg;
		pthread_mutex_lock(da->writelock);
		while (*(da->videoWritten) == false)
		{
			pthread_cond_wait(da->writecond, da->writelock);
		}
		// use ffmpeg to turn video into images
		pid_t p = fork();
		if (p == 0)
		{
			close(STDOUT_FILENO); close(STDERR_FILENO); // close writing
			execlp("ffmpeg", "ffmpeg", "-i", da->videoFileName, "-r", "24", da->outname[da->writebuffidx], NULL);
			perror("");
			exit(12);
		}
		waitpid(p, NULL, 0);
		// load filenames into struct
		da->nameListLengths[da->writebuffidx] = 0;
		DIR* dirp = opendir(da->outdir[da->writebuffidx]);
		struct dirent* entry;
		while ((entry = readdir(dirp)) != NULL)
		{
			if (entry->d_type == DT_REG)
			da->nameListLengths[da->writebuffidx]++;
		}
		rewinddir(dirp);
		char** strptr = da->filenameLists[da->writebuffidx]
			= malloc(da->nameListLengths[da->writebuffidx] * sizeof(char*));
		int i = 0;
		while ((entry = readdir(dirp)) != NULL)
		{
			if (entry->d_type == DT_REG)
			{
				strptr[i] = malloc(strlen(entry->d_name)+1);
				strcpy(strptr[i], da->outdir[da->writebuffidx]);
				strcat(strptr[i], entry->d_name);
				i++;
			}
		}
		*(da->videoWritten) = false;
		pthread_cond_signal(da->writecond);
		pthread_mutex_unlock(da->writelock);
		da->writebuffidx = (da->writebuffidx+1)%2;
		pthread_barrier_wait(da->barrier);
	}
	return NULL;
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
	//SDL_Texture* streamTexture = SDL_CreateTexture(renderer,
	//	SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 640, 480);
	//texture = SDL_CreateTextureFromSurface(renderer, image);

	// I want to show the n images evenly over 1 second
	//	int pitch = 480;
	struct timespec screendur;
	bool quit = false;
	int buffidx;
	pthread_barrier_wait(da->barrier);
	while (!quit)
	{
		/*
		pthread_mutex_lock(da->writelock);
		while (*da->videoWritten == false)
		{
			pthread_cond_wait(da->writecond, da->writelock);
		}
		pthread_mutex_unlock(da->writelock);
		*/

		if (da->writebuffidx == 1) // we're gonna read from buffer 0
			buffidx = 0;
		else if (da->writebuffidx == 0) // we're gonna read from buffer 1
			buffidx = 1;
		// buffidx = (buffidx+1)%2;
		if (da->nameListLengths[buffidx] == 0) continue;
		screendur.tv_nsec = 1000000000 / da->nameListLengths[buffidx];

		int imgidx; for (imgidx = 0; imgidx < da->nameListLengths[buffidx]; imgidx++)
		{
			// @TODO clean up texture loading to avoid memory leaks
			image = IMG_Load(da->filenameLists[buffidx][imgidx]);
			//SDL_LockTexture(streamTexture, NULL, (void**)image, &pitch);

			//SDL_UnlockTexture(streamTexture);
			texture = SDL_CreateTextureFromSurface(renderer, image);
			//SDL_WaitEvent(&event);

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
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(image);
		}
		/*
		pid_t p = fork();
		if (p == 0)
		{
			execlp("rm", "rm", da->outdirstar[buffidx], NULL);
			perror("");
			exit(6);
		}
		waitpid(p, NULL, 0);
		*/

		pthread_barrier_wait(da->barrier);
	}

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	 	IMG_Quit();
	SDL_Quit();

	return NULL;
}
