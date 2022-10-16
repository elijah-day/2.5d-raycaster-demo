#include <stdbool.h>
#include <dirent.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#define PI 3.14

#define ROTATION_SPEED PI / 128
#define MOVEMENT_SPEED 0.05

#define VIEWLINE_LENGTH 64
#define WALL_SIZE 4

#define RAY_COUNT 64
#define RAY_ANGLE (PI / ((96 / 32) * RAY_COUNT))

#define RENDER_SCALE 8
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

typedef struct Wall
{
	int x, y;
}
Wall;

typedef struct Entity
{
	float x, y, theta;
}
Entity;

int main(int argc, char *argv[])
{
	/*Misc. dirent testing.*/

	/*
	const char *dirstr = "../../";

	struct dirent *entry = NULL;
	DIR *dir = opendir(dirstr);

	if(dir == NULL)
	{
		printf("opendir error\n");
		return 1;
	}
	
	while((entry = readdir(dir)) != NULL)
	{
		if(entry)
		{
			char fstr[64];
			strcpy(fstr, dirstr);
			strcat(fstr, entry->d_name);
			printf("%s\n", fstr);
			FILE *fp = fopen(fstr, "r");
			
			if(fp != NULL)
			{
				printf("%c\n", fgetc(fp));
				fclose(fp);
			}
		}
	}
	
	closedir(dir);
	*/
	
	bool running_state = true;
	
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init error\n");
		running_state = false;
	}
	
	SDL_Window *window = SDL_CreateWindow
	(
		"",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	
	if(window == NULL)
	{
		printf("SDL_CreateWindow error\n");	
		running_state = false;
	}
	
	SDL_Renderer *renderer = SDL_CreateRenderer
	(
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	);
	
	if(renderer == NULL)
	{
		printf("SDL_CreateRenderer error\n");
		running_state = false;
	}
	
	int wall_count = 0;
	
	bool room[8][8] =
	{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 1, 0, 1, 1},
		{0, 1, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 1, 0, 0, 1},
		{0, 0, 0, 0, 1, 0, 1, 0},
		{0, 0, 0, 0, 1, 0, 0, 0},
		{0, 1, 0, 0, 1, 0, 0, 1}
	};
	
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(room[i][j] == true) wall_count++;
		}
	}
	
	Wall walls[wall_count];
	
	int wcounter = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(room[i][j] == true)
			{
				walls[wcounter].y = i * WALL_SIZE;
				walls[wcounter].x = j * WALL_SIZE;
				wcounter++;
			}
		}
	}
	
	printf("%d\n", wall_count);
	
	Entity player;
	player.x= 0;
	player.y = 0;
	player.theta = 0;
	
	bool minimap_toggle = true;
	int win_w = 0;
	int win_h = 0;
	
	int wall_line_scale = 96;
	
	int brightness = 2;
	
	bool sdl_keys[322];
	memset(sdl_keys, false, 322);
	
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	SDL_Event event;
	while(running_state) while(SDL_PollEvent(&event))
	{
		float ppx = player.x;
		float ppy = player.y;
	
		SDL_GetWindowSize(window, &win_w, &win_h);
		
		switch(event.type)
		{
			case SDL_QUIT:
				running_state = false;
				break;
				
			case SDL_KEYDOWN:
				sdl_keys[event.key.keysym.scancode] = true;
				
				switch(event.key.keysym.scancode)
				{
					case SDL_SCANCODE_M:
						minimap_toggle = !minimap_toggle;
						break;
						
					case SDL_SCANCODE_O:
						wall_line_scale -= 4;
						break;
					
					case SDL_SCANCODE_P:
						wall_line_scale += 4;
						break;
						
					case SDL_SCANCODE_K:
						brightness -= 1;
						break;
						
					case SDL_SCANCODE_L:
						brightness += 1;
						break;
				}
				
				break;
				
			case SDL_KEYUP:
				sdl_keys[event.key.keysym.scancode] = false;
				break;
		}
		
		if(sdl_keys[SDL_SCANCODE_ESCAPE]) running_state = 0;
		
		if(sdl_keys[SDL_SCANCODE_LEFT]) player.theta += ROTATION_SPEED;
		if(sdl_keys[SDL_SCANCODE_RIGHT]) player.theta -= ROTATION_SPEED;
		
		if(sdl_keys[SDL_SCANCODE_W])
		{
			player.x += MOVEMENT_SPEED * cos(player.theta);
			player.y -= MOVEMENT_SPEED * sin(player.theta);
		}
		if(sdl_keys[SDL_SCANCODE_A])
		{
			player.x -= MOVEMENT_SPEED * cos(player.theta - PI / 2);
			player.y += MOVEMENT_SPEED * sin(player.theta - PI / 2);
		}
		if(sdl_keys[SDL_SCANCODE_S])
		{
			player.x -= MOVEMENT_SPEED * cos(player.theta);
			player.y += MOVEMENT_SPEED * sin(player.theta);
		}
		if(sdl_keys[SDL_SCANCODE_D])
		{
			player.x -= MOVEMENT_SPEED * cos(player.theta + PI / 2);
			player.y += MOVEMENT_SPEED * sin(player.theta + PI / 2);
		}
		
		/*Draw BG*/
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
		SDL_RenderFillRect(renderer, NULL);
		
		/*Draw Floor*/
		for(int k = 1; k < win_h / 2; k++)
		{
			SDL_SetRenderDrawColor(renderer, win_h / k, win_h / k, win_h / k, 255);
		
			SDL_RenderDrawLine
			(
				renderer,
				0,
				win_h / 2 + k,
				win_w,
				win_h / 2 + k
			);
		}
		
		/*Raycasting*/
		for(int k = 0; k < RAY_COUNT; k++)
		{	
			float rx = player.x;
			float ry = player.y;
			float rlength = 0;
			float rtheta = player.theta;
			bool ray_hit_state = false;
			SDL_Rect wlrect;
		
			while(!ray_hit_state && rlength < VIEWLINE_LENGTH)
			{
				rtheta = player.theta + (RAY_COUNT / 2 - k) * RAY_ANGLE;
			
				rx += cos(rtheta);
				ry -= sin(rtheta);
				
				for(int i = 0; i < wall_count; i++)
				{
					if
					(
						rx > (float)(walls[i].x) && rx < (float)(walls[i].x) + (float)WALL_SIZE &&
						ry > (float)(walls[i].y) && ry < (float)(walls[i].y) + (float)WALL_SIZE
					)
					{
						ray_hit_state = true;
						/*
						printf("RAY HIT!!!\n");
						*/
						break;
					}
				}
				
				rlength += 1;
			}
			
			SDL_SetRenderDrawColor
			(
				renderer,
				brightness * 255 / rlength,
				brightness * 255 / rlength,
				brightness * 255 / rlength,
				255
			);
			
			if(rlength < VIEWLINE_LENGTH)
			{
				int wall_line_width = (int)((float)win_w / (float)RAY_COUNT);
			
				for(int i = 0; i < wall_line_width; i++) SDL_RenderDrawLine
				(
					renderer,
					win_w  - (RAY_COUNT - k) * wall_line_width + i,
					win_h / 2 + RENDER_SCALE * wall_line_scale / rlength,
					win_w - (RAY_COUNT - k) * wall_line_width + i,
					win_h / 2 - RENDER_SCALE * wall_line_scale / rlength
				);
			}
			
			if(minimap_toggle) SDL_RenderDrawLine
			(
				renderer,
				RENDER_SCALE * (int)player.x,
				RENDER_SCALE * (int)player.y,
				RENDER_SCALE * (int)player.x + rlength * cos(rtheta) * RENDER_SCALE,
				RENDER_SCALE * (int)player.y - rlength * sin(rtheta) * RENDER_SCALE
			);
		}
		
		if(minimap_toggle)
		{
			SDL_Rect wall_rect;
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			for(int i = 0; i < wall_count; i++)
			{
				wall_rect.x = walls[i].x * RENDER_SCALE;
				wall_rect.y = walls[i].y * RENDER_SCALE;
				wall_rect.w = WALL_SIZE * RENDER_SCALE;
				wall_rect.h = WALL_SIZE * RENDER_SCALE;
				
				SDL_RenderFillRect(renderer, &wall_rect);
			}
		}
		
		SDL_RenderPresent(renderer);
		
		/*
		printf("player.theta=%f\n", player.theta);
		*/
		
		if(player.theta > 2 * PI + PI / 48) player.theta = 0;
		if(player.theta < 0) player.theta = 2 * PI;
		
		
		/*
		printf("\n");
		*/
	}
	
	SDL_SetWindowFullscreen(window, 0);
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}
